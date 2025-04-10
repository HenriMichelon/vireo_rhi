/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "backend/vulkan/Tools.h"
module vireo.backend.vulkan.resources;

namespace vireo::backend {

    VKBuffer::VKBuffer(
            const VKDevice& device,
            const Type type,
            const size_t size,
            const size_t count,
            const size_t minOffsetAlignment,
            const std::wstring& name) : Buffer{type},device{device} {
        alignmentSize = minOffsetAlignment > 0
               ? (size + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1)
               : size;
        bufferSize = alignmentSize * count;

        const VkBufferCreateFlags usage =
            type == VERTEX ? VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT :
            type == INDEX ? VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT:
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        const auto memTypeIndex = (type == VERTEX || type == INDEX) ?
            device.getPhysicalDevice().getMemoryTypeDeviceLocalIndex() :
            device.getPhysicalDevice().getMemoryTypeHostVisibleIndex();
        createBuffer(device, bufferSize, usage, memTypeIndex, buffer, bufferMemory);
        vkSetObjectName(device.getDevice(), reinterpret_cast<uint64_t>(buffer), VK_OBJECT_TYPE_BUFFER, wstring_to_string(name));
    }

    void VKBuffer::map() {
        vkMapMemory(device.getDevice(), bufferMemory, 0, bufferSize, 0, &mappedAddress);
    }

    void VKBuffer::unmap() {
        vkUnmapMemory(device.getDevice(), bufferMemory);
        mappedAddress = nullptr;
    }

    void VKBuffer::write(const void* data, const size_t size, const size_t offset) {
        if (size == WHOLE_SIZE) {
            memcpy(mappedAddress, data, bufferSize);
        } else {
            memcpy(static_cast<unsigned char*>(mappedAddress) + offset, data, size);
        }
    }

    void VKBuffer::createBuffer(
            const VKDevice& device,
            const VkDeviceSize size,
            const VkBufferUsageFlags usage,
            const uint32_t memoryTypeIndex,
            VkBuffer& buffer,
            VkDeviceMemory& memory) {
        const auto bufferInfo = VkBufferCreateInfo {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size = size,
            .usage = usage,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        };
        DieIfFailed (vkCreateBuffer(device.getDevice(), &bufferInfo, nullptr, &buffer));
        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(device.getDevice(), buffer, &memRequirements);
        const auto allocInfo = VkMemoryAllocateInfo {
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .allocationSize = memRequirements.size,
            .memoryTypeIndex = memoryTypeIndex
        };
        DieIfFailed(vkAllocateMemory(device.getDevice(), &allocInfo, nullptr, &memory));
        vkBindBufferMemory(device.getDevice(), buffer, memory, 0);
    }

    VKBuffer::~VKBuffer() {
        vkDestroyBuffer(device.getDevice(), buffer, nullptr);
        vkFreeMemory(device.getDevice(), bufferMemory, nullptr);
    }


    VKSampler::VKSampler(
        const VKPhysicalDevice& physicalDevice,
        const VkDevice device,
        const Filter minFilter,
        const Filter magFilter,
        const AddressMode addressModeU,
        const AddressMode addressModeV,
        const AddressMode addressModeW,
        const float minLod,
        const float maxLod,
        const bool anisotropyEnable,
        const MipMapMode mipMapMode) :
        device{device} {
        // https://vulkan-tutorial.com/Texture_mapping/Image_view_and_sampler#page_Samplers
        const auto samplerInfo = VkSamplerCreateInfo {
            .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
            .magFilter = static_cast<VkFilter>(magFilter),
            .minFilter = static_cast<VkFilter>(minFilter),
            .mipmapMode = static_cast<VkSamplerMipmapMode>(mipMapMode),
            .addressModeU = static_cast<VkSamplerAddressMode>(addressModeU),
            .addressModeV = static_cast<VkSamplerAddressMode>(addressModeV),
            .addressModeW = static_cast<VkSamplerAddressMode>(addressModeW),
            .mipLodBias = 0.0f,
            .anisotropyEnable = anisotropyEnable,
            // https://vulkan-tutorial.com/Texture_mapping/Image_view_and_sampler#page_Anisotropy-device-feature
            .maxAnisotropy = physicalDevice.getDeviceProperties().limits.maxSamplerAnisotropy,
            .compareEnable = VK_FALSE,
            .compareOp = VK_COMPARE_OP_ALWAYS,
            .minLod = minLod,
            .maxLod = maxLod,
            .borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK,
            .unnormalizedCoordinates = VK_FALSE,
        };
        DieIfFailed(vkCreateSampler(device, &samplerInfo, nullptr, &sampler));
    }

    VKSampler::~VKSampler() {
        vkDestroySampler(device, sampler, nullptr);
    }

}