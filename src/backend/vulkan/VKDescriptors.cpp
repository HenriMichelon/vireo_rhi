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

    VKDescriptorSet::VKDescriptorSet(const DescriptorType type, VkDevice device, size_t capacity, const std::wstring& name):
        DescriptorSet{type, capacity},
        device{device} {
        const VkDescriptorType vkType =
            type == DescriptorType::BUFFER ? VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER :
            VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;

        const auto poolSize = VkDescriptorPoolSize {
            .type =vkType,
            .descriptorCount = static_cast<uint32_t>(capacity)
        };
        const auto poolInfo = VkDescriptorPoolCreateInfo {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .maxSets = static_cast<uint32_t>(capacity),
            .poolSizeCount = 1,
            .pPoolSizes = &poolSize,
        };
        vkCreateDescriptorPool(device, &poolInfo, nullptr, &pool);

        const auto binding = VkDescriptorSetLayoutBinding {
            .binding = 0,
            .descriptorType = vkType,
            .descriptorCount = static_cast<uint32_t>(capacity),
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
        };
        const auto layoutInfo = VkDescriptorSetLayoutCreateInfo {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .bindingCount = 1,
            .pBindings = &binding,
        };
        vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &setLayout);
        vkSetObjectName(device, reinterpret_cast<uint64_t>(setLayout), VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT,
            wstring_to_string(L"Set Layout : " + name).c_str());

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
        vkDestroyDescriptorSetLayout(device, setLayout, nullptr);
    }

    void VKDescriptorSet::update(const DescriptorHandle handle, Buffer& buffer) {
        assert(type == DescriptorType::BUFFER);
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
        vkUpdateDescriptorSets(device, 1, &write, 0, nullptr);
    }

    // void VKDescriptorSet::update(const DescriptorHandle handle, Sampler& sampler) {
    //     assert(type == DescriptorType::SAMPLER);
    //     const auto& vkSampler = static_cast<VKSampler&>(sampler);
    //     const auto imageInfo = VkDescriptorImageInfo {
    //         .sampler = vkSampler.getSampler(),
    //         .imageView = VK_NULL_HANDLE,
    //         .imageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    //     };
    //     const auto write = VkWriteDescriptorSet {
    //         .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
    //         .dstSet = set,
    //         .dstBinding = 0,
    //         .dstArrayElement = handle,
    //         .descriptorCount = 1,
    //         .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER,
    //         .pImageInfo = &imageInfo,
    //     };
    //     vkUpdateDescriptorSets(device, 1, &write, 0, nullptr);
    // }

}