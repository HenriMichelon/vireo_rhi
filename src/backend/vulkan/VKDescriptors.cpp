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

    VKDescriptorLayout::VKDescriptorLayout(const VkDevice device, const std::wstring& name):
        device{device}, name{name} {
    }

    DescriptorLayout& VKDescriptorLayout::add(const DescriptorIndex index, const std::vector<std::shared_ptr<Sampler>>& staticSamplers) {
        poolSizes[index] = {
             .type = VK_DESCRIPTOR_TYPE_SAMPLER,
             .descriptorCount = static_cast<uint32_t>(staticSamplers.size()),
        };
        this->staticSamplers.resize(staticSamplers.size());
        for (int i = 0; i < staticSamplers.size(); i++) {
            this->staticSamplers[i] = static_pointer_cast<VKSampler>(staticSamplers[i])->getSampler();
        }
        capacity += staticSamplers.size();
        return *this;
    }

    DescriptorLayout& VKDescriptorLayout::add(const DescriptorIndex index, const DescriptorType type, const size_t count) {
        poolSizes[index] = {
            .type = type == DescriptorType::BUFFER ? VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER : VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
            .descriptorCount = static_cast<uint32_t>(count),
        };
        capacity += count;
        return *this;
    }

    void VKDescriptorLayout::build() {
        std::vector<VkDescriptorSetLayoutBinding> bindings;
        for (const auto& poolSize : poolSizes) {
            auto binding = VkDescriptorSetLayoutBinding{
                .binding = poolSize.first,
                .descriptorType = poolSize.second.type,
                .descriptorCount = poolSize.second.descriptorCount,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            };
            if (poolSize.second.type == VK_DESCRIPTOR_TYPE_SAMPLER) { // assuming we only have static samplers
                binding.pImmutableSamplers = staticSamplers.data();
            }
            bindings.push_back(binding);
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
        vkDestroyDescriptorSetLayout(device, setLayout, nullptr);
    }

    VKDescriptorSet::VKDescriptorSet(
        const DescriptorLayout& layout,
        const std::wstring& name):
        DescriptorSet {layout} {
        const auto& vkLayout = static_cast<const VKDescriptorLayout&>(layout);
        const auto setLayout = vkLayout.getSetLayout();
        device = vkLayout.getDevice();

        std::vector<VkDescriptorPoolSize> poolSizes;
        for (const auto &poolSize : vkLayout.getPoolSizes()) {
            poolSizes.push_back(poolSize.second);
        }
        const auto poolInfo = VkDescriptorPoolCreateInfo {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .maxSets = static_cast<uint32_t>(vkLayout.getCapacity()),
            .poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
            .pPoolSizes = poolSizes.data(),
        };
        vkCreateDescriptorPool(device, &poolInfo, nullptr, &pool);
        vkSetObjectName(device, reinterpret_cast<uint64_t>(pool), VK_OBJECT_TYPE_DESCRIPTOR_POOL,
             wstring_to_string(L"Pool : " + name).c_str());

        const auto allocInfo = VkDescriptorSetAllocateInfo {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = pool,
            .descriptorSetCount = 1,
            .pSetLayouts = &setLayout,
        };
        vkAllocateDescriptorSets(device, &allocInfo, &set);
        vkSetObjectName(device, reinterpret_cast<uint64_t>(set), VK_OBJECT_TYPE_DESCRIPTOR_SET,
            wstring_to_string(L"Set : " + name).c_str());
    }

    VKDescriptorSet::~VKDescriptorSet() {
        vkDestroyDescriptorPool(device, pool, nullptr);
        // vkFreeDescriptorSets(static_cast<const VKDescriptorLayout&>(layout).getDevice(), set, nullptr);
    }

    void VKDescriptorSet::update(const DescriptorIndex index, Buffer& buffer) {
        const auto& vkBuffer = static_cast<VKBuffer&>(buffer);
        const auto bufferInfo = VkDescriptorBufferInfo {
            .buffer = vkBuffer.getBuffer(),
            .range = vkBuffer.getSize(),
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
        vkUpdateDescriptorSets(static_cast<const VKDescriptorLayout&>(layout).getDevice(), 1, &write, 0, nullptr);
    }

    void VKDescriptorSet::update(const DescriptorIndex index, Image& sampler) {
        const auto& vkImage = static_cast<VKImage&>(sampler);
        const auto imageInfo = VkDescriptorImageInfo {
            .sampler = VK_NULL_HANDLE,
            .imageView = vkImage.getImageView(),
            .imageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
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
        vkUpdateDescriptorSets(static_cast<const VKDescriptorLayout&>(layout).getDevice(), 1, &write, 0, nullptr);
    }

}