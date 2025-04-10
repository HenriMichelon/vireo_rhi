/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "backend/vulkan/Tools.h"
module vireo.backend.vulkan.descriptors;

import vireo.backend.vulkan.resources;

namespace vireo::backend {

    VKDescriptorLayout::VKDescriptorLayout(
            DescriptorType type,
            VkDevice device,
            size_t capacity,
            const std::vector<std::shared_ptr<Sampler>>& staticSamplers,
            const std::wstring& name):
        DescriptorLayout{type, capacity},
        device{device} {
        const VkDescriptorType vkType =
            type == DescriptorType::BUFFER ? VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER :
            VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;

        std::vector<VkDescriptorPoolSize> poolSizes;
        poolSizes.push_back({
            .type = vkType,
            .descriptorCount = static_cast<uint32_t>(capacity),
        });
        if (!staticSamplers.empty()) {
            poolSizes.push_back({
                .type = VK_DESCRIPTOR_TYPE_SAMPLER,
                .descriptorCount = static_cast<uint32_t>(staticSamplers.size()),
            });
        }

        const auto poolInfo = VkDescriptorPoolCreateInfo {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .maxSets = static_cast<uint32_t>(capacity),
            .poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
            .pPoolSizes = poolSizes.data(),
        };
        vkCreateDescriptorPool(device, &poolInfo, nullptr, &pool);
        vkSetObjectName(device, reinterpret_cast<uint64_t>(pool), VK_OBJECT_TYPE_DESCRIPTOR_POOL,
             wstring_to_string(L"Pool : " + name).c_str());

        std::vector<VkDescriptorSetLayoutBinding> bindings;
        bindings.push_back({
            .binding = 0,
            .descriptorType = vkType,
            .descriptorCount = static_cast<uint32_t>(capacity),
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
        });

        std::vector<VkSampler> samplers(staticSamplers.size());
        if (!staticSamplers.empty()) {
            for (int i = 0; i < staticSamplers.size(); i++) {
                samplers[i] = static_pointer_cast<VKSampler>(staticSamplers[i])->getSampler();
            }
            bindings.push_back({
                .binding = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER,
                .descriptorCount = static_cast<uint32_t>(samplers.size()),
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                .pImmutableSamplers = samplers.data()
            });
        }

        const auto layoutInfo = VkDescriptorSetLayoutCreateInfo {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .bindingCount = static_cast<uint32_t>(bindings.size()),
            .pBindings = bindings.data(),
        };
        vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &setLayout);
        vkSetObjectName(device, reinterpret_cast<uint64_t>(setLayout), VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT,
            wstring_to_string(L"Set Layout : " + name).c_str());
    }

    VKDescriptorLayout::~VKDescriptorLayout() {
        vkDestroyDescriptorPool(device, pool, nullptr);
        vkDestroyDescriptorSetLayout(device, setLayout, nullptr);
    }

    VKDescriptorSet::VKDescriptorSet(
        const DescriptorLayout& layout,
        const std::wstring& name):
        DescriptorSet {layout} {
        const auto& vkLayout = static_cast<const VKDescriptorLayout&>(layout);
        const auto setLayout = vkLayout.getSetLayout();
        const auto allocInfo = VkDescriptorSetAllocateInfo {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = vkLayout.getPool(),
            .descriptorSetCount = 1,
            .pSetLayouts = &setLayout,
        };
        vkAllocateDescriptorSets(vkLayout.getDevice(), &allocInfo, &set);
        vkSetObjectName(vkLayout.getDevice(), reinterpret_cast<uint64_t>(set), VK_OBJECT_TYPE_DESCRIPTOR_SET,
            wstring_to_string(L"Set : " + name).c_str());
    }

    VKDescriptorSet::~VKDescriptorSet() {
        // vkFreeDescriptorSets(static_cast<const VKDescriptorLayout&>(layout).getDevice(), set, nullptr);
    }

    void VKDescriptorSet::update(const DescriptorHandle handle, Buffer& buffer) {
        assert(layout.getType() == DescriptorType::BUFFER);
        const auto& vkBuffer = static_cast<VKBuffer&>(buffer);
        const auto bufferInfo = VkDescriptorBufferInfo {
            .buffer = vkBuffer.getBuffer(),
            .range = vkBuffer.getSize(),
        };
        const auto write = VkWriteDescriptorSet {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = set,
            .dstBinding = 0,
            .dstArrayElement = handle,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .pBufferInfo = &bufferInfo,
        };
        vkUpdateDescriptorSets(static_cast<const VKDescriptorLayout&>(layout).getDevice(), 1, &write, 0, nullptr);
    }

    void VKDescriptorSet::update(std::vector<DescriptorHandle> handles, const std::vector<std::shared_ptr<Buffer>>& buffer) {
        assert(layout.getType() == DescriptorType::BUFFER);
        assert(handles.size() == buffer.size());

        std::vector<VkWriteDescriptorSet> writes(handles.size());
        std::vector<VkDescriptorBufferInfo> buffersInfo(handles.size());

        for (int i = 0; i < handles.size(); i++) {
            const auto vkBuffer = static_pointer_cast<VKBuffer>(buffer[i]);
            buffersInfo[i].buffer = vkBuffer->getBuffer();
            buffersInfo[i].range = vkBuffer->getSize();
            writes[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writes[i].dstSet = set;
            writes[i].dstBinding = 0;
            writes[i].dstArrayElement = handles[i];
            writes[i].descriptorCount = 1;
            writes[i].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            writes[i].pBufferInfo = &buffersInfo[i];
        }

        vkUpdateDescriptorSets(static_cast<const VKDescriptorLayout&>(layout).getDevice(), writes.size(), writes.data(), 0, nullptr);
    }

    void VKDescriptorSet::update(const DescriptorHandle handle, Image& sampler) {
        assert(layout.getType() == DescriptorType::TEXTURE);
        const auto& vkImage = static_cast<VKImage&>(sampler);
        const auto imageInfo = VkDescriptorImageInfo {
            .sampler = VK_NULL_HANDLE,
            .imageView = vkImage.getImageView(),
            .imageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        };
        const auto write = VkWriteDescriptorSet {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = set,
            .dstBinding = 0,
            .dstArrayElement = handle,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
            .pImageInfo = &imageInfo,
        };
        vkUpdateDescriptorSets(static_cast<const VKDescriptorLayout&>(layout).getDevice(), 1, &write, 0, nullptr);
    }

}