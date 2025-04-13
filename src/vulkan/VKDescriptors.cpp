/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/backend/vulkan/Tools.h"
module vireo.vulkan.descriptors;

import vireo.vulkan.resources;

namespace vireo {

    VKDescriptorLayout::VKDescriptorLayout(const VkDevice device, const wstring& name):
        device{device}, name{name} {
    }

    DescriptorLayout& VKDescriptorLayout::add(const DescriptorIndex index, const DescriptorType type, const size_t count) {
        poolSizes[index] = {
            .type =
                type == DescriptorType::BUFFER ? VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER :
                type == DescriptorType::IMAGE ? VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE :
                VK_DESCRIPTOR_TYPE_SAMPLER,
            .descriptorCount = static_cast<uint32_t>(count),
        };
        capacity += count;
        return *this;
    }

    void VKDescriptorLayout::build() {
        vector<VkDescriptorSetLayoutBinding> bindings;
        for (const auto& poolSize : poolSizes) {
            auto binding = VkDescriptorSetLayoutBinding{
                .binding = poolSize.first,
                .descriptorType = poolSize.second.type,
                .descriptorCount = poolSize.second.descriptorCount,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            };
            bindings.push_back(binding);
        }

        const auto layoutInfo = VkDescriptorSetLayoutCreateInfo {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .bindingCount = static_cast<uint32_t>(bindings.size()),
            .pBindings = bindings.data(),
        };
        vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &setLayout);
#ifdef _DEBUG
        vkSetObjectName(device, reinterpret_cast<uint64_t>(setLayout), VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT,
            wstring_to_string(L"VKDescriptorLayout : " + name).c_str());
#endif
    }

    VKDescriptorLayout::~VKDescriptorLayout() {
        vkDestroyDescriptorSetLayout(device, setLayout, nullptr);
    }

    VKDescriptorSet::VKDescriptorSet(
        const shared_ptr<DescriptorLayout>& layout,
        const wstring& name):
        DescriptorSet {layout} {
        const auto vkLayout = static_pointer_cast<const VKDescriptorLayout>(layout);
        const auto setLayout = vkLayout->getSetLayout();
        device = vkLayout->getDevice();

        vector<VkDescriptorPoolSize> poolSizes;
        for (const auto &poolSize : vkLayout->getPoolSizes()) {
            poolSizes.push_back(poolSize.second);
        }
        const auto poolInfo = VkDescriptorPoolCreateInfo {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .maxSets = static_cast<uint32_t>(vkLayout->getCapacity()),
            .poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
            .pPoolSizes = poolSizes.data(),
        };
        vkCreateDescriptorPool(device, &poolInfo, nullptr, &pool);

        const auto allocInfo = VkDescriptorSetAllocateInfo {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = pool,
            .descriptorSetCount = 1,
            .pSetLayouts = &setLayout,
        };
        vkAllocateDescriptorSets(device, &allocInfo, &set);

#ifdef _DEBUG
        vkSetObjectName(device, reinterpret_cast<uint64_t>(pool), VK_OBJECT_TYPE_DESCRIPTOR_POOL,
             wstring_to_string(L"VKDescriptorSet Pool : " + name).c_str());
        vkSetObjectName(device, reinterpret_cast<uint64_t>(set), VK_OBJECT_TYPE_DESCRIPTOR_SET,
            wstring_to_string(L"VKDescriptorSet : " + name).c_str());
#endif
    }

    VKDescriptorSet::~VKDescriptorSet() {
        vkDestroyDescriptorPool(device, pool, nullptr);
        // vkFreeDescriptorSets(static_cast<const VKDescriptorLayout&>(layout).getDevice(), set, nullptr);
    }

    void VKDescriptorSet::update(const DescriptorIndex index, const shared_ptr<Buffer>& buffer) {
        const auto vkBuffer = static_pointer_cast<VKBuffer>(buffer);
        const auto bufferInfo = VkDescriptorBufferInfo {
            .buffer = vkBuffer->getBuffer(),
            .range = vkBuffer->getSize(),
        };
        const auto write = VkWriteDescriptorSet {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = set,
            .dstBinding = index,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .pBufferInfo = &bufferInfo,
        };
        vkUpdateDescriptorSets(static_pointer_cast<const VKDescriptorLayout>(layout)->getDevice(), 1, &write, 0, nullptr);
    }

    void VKDescriptorSet::update(const DescriptorIndex index, const shared_ptr<Image>& image) {
        const auto vkImage = static_pointer_cast<VKImage>(image);
        const auto imageInfo = VkDescriptorImageInfo {
            .sampler = VK_NULL_HANDLE,
            .imageView = vkImage->getImageView(),
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        };
        const auto write = VkWriteDescriptorSet {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = set,
            .dstBinding = index,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
            .pImageInfo = &imageInfo,
        };
        vkUpdateDescriptorSets(static_pointer_cast<const VKDescriptorLayout>(layout)->getDevice(), 1, &write, 0, nullptr);
    }

    void VKDescriptorSet::update(const DescriptorIndex index, const shared_ptr<Sampler>& sampler) {
        const auto vkSampler = static_pointer_cast<VKSampler>(sampler);
        const auto imageInfo = VkDescriptorImageInfo {
            .sampler = vkSampler->getSampler(),
            .imageView = VK_NULL_HANDLE,
            .imageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        };
        const auto write = VkWriteDescriptorSet {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = set,
            .dstBinding = index,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER,
            .pImageInfo = &imageInfo,
        };
        vkUpdateDescriptorSets(static_pointer_cast<const VKDescriptorLayout>(layout)->getDevice(), 1, &write, 0, nullptr);
    }

    void VKDescriptorSet::update(const DescriptorIndex index, const vector<shared_ptr<Buffer>>& buffers) {
        auto buffersInfo = vector<VkDescriptorBufferInfo>(buffers.size());
        for (int i = 0; i < buffers.size(); i++) {
            const auto& vkBuffer = static_pointer_cast<VKBuffer>(buffers[i]);
            buffersInfo[i].buffer = vkBuffer->getBuffer();
            buffersInfo[i].range = vkBuffer->getSize();
        }
        const auto write = VkWriteDescriptorSet {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = set,
            .dstBinding = index,
            .dstArrayElement = 0,
            .descriptorCount = static_cast<uint32_t>(buffersInfo.size()),
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .pBufferInfo = buffersInfo.data(),
        };
        vkUpdateDescriptorSets(static_pointer_cast<const VKDescriptorLayout>(layout)->getDevice(), 1, &write, 0, nullptr);
    }

    void VKDescriptorSet::update(const DescriptorIndex index, const vector<shared_ptr<Image>>& images) {
        auto imagesInfo = vector<VkDescriptorImageInfo>(images.size());
        for (int i = 0; i < images.size(); i++) {
            imagesInfo[i].sampler = VK_NULL_HANDLE;
            imagesInfo[i].imageView = static_pointer_cast<VKImage>(images[i])->getImageView();
            imagesInfo[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        }
        const auto write = VkWriteDescriptorSet {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = set,
            .dstBinding = index,
            .dstArrayElement = 0,
            .descriptorCount = static_cast<uint32_t>(imagesInfo.size()),
            .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
            .pImageInfo = imagesInfo.data(),
        };
        vkUpdateDescriptorSets(static_pointer_cast<const VKDescriptorLayout>(layout)->getDevice(), 1, &write, 0, nullptr);
    }

    void VKDescriptorSet::update(const DescriptorIndex index, const vector<shared_ptr<Sampler>>&samplers) {
        auto imagesInfo = vector<VkDescriptorImageInfo>(samplers.size());
        for (int i = 0; i < samplers.size(); i++) {
            imagesInfo[i].sampler = static_pointer_cast<VKSampler>(samplers[i])->getSampler();
            imagesInfo[i].imageView = VK_NULL_HANDLE;
            imagesInfo[i].imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        }
        const auto write = VkWriteDescriptorSet {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = set,
            .dstBinding = index,
            .dstArrayElement = 0,
            .descriptorCount = static_cast<uint32_t>(imagesInfo.size()),
            .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER,
            .pImageInfo = imagesInfo.data(),
        };
        vkUpdateDescriptorSets(static_pointer_cast<const VKDescriptorLayout>(layout)->getDevice(), 1, &write, 0, nullptr);

    }
}