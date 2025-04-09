/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "backend/vulkan/Tools.h"
module vireo.backend.vulkan.descriptors;

import vireo.backend.vulkan.buffer;

namespace vireo::backend {

    VKDescriptorAllocator::VKDescriptorAllocator(const DescriptorType type, VkDevice device, uint32_t capacity):
        DescriptorAllocator{type, capacity},
        device{device} {
        const auto poolSize = VkDescriptorPoolSize {
            .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = capacity
        };
        const auto poolInfo = VkDescriptorPoolCreateInfo {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .maxSets = capacity,
            .poolSizeCount = 1,
            .pPoolSizes = &poolSize,
        };
        vkCreateDescriptorPool(device, &poolInfo, nullptr, &pool);

        const auto binding = VkDescriptorSetLayoutBinding {
            .binding = 0,
            .descriptorType = type == DescriptorType::BUFFER ?
                VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER :
                VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
            .descriptorCount = capacity,
            .stageFlags = VK_SHADER_STAGE_ALL,
        };

        const auto layoutInfo = VkDescriptorSetLayoutCreateInfo {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .bindingCount = 1,
            .pBindings = &binding,
        };
        vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &setLayout);

        const auto allocInfo = VkDescriptorSetAllocateInfo {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = pool,
            .descriptorSetCount = 1,
            .pSetLayouts = &setLayout,
        };
        vkAllocateDescriptorSets(device, &allocInfo, &set);
    }

    VKDescriptorAllocator::~VKDescriptorAllocator() {
        vkDestroyDescriptorPool(device, pool, nullptr);
        vkDestroyDescriptorSetLayout(device, setLayout, nullptr);
    }

    void VKDescriptorAllocator::update(DescriptorHandle handle, Buffer& buffer) {
        assert(type == DescriptorType::BUFFER);
        const auto vkBuffer = static_cast<VKBuffer&>(buffer);
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
        vkUpdateDescriptorSets(device, 1, &write, 0, nullptr);
    }

    uint64_t VKDescriptorAllocator::getGPUHandle(DescriptorHandle handle) const {
        return handle;
    }

}