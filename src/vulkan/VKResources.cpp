/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/backend/vulkan/Libraries.h"
module vireo.vulkan.resources;

import vireo.tools;

import vireo.vulkan.tools;

namespace vireo {

    VKBuffer::VKBuffer(
            const std::shared_ptr<const VKDevice>& device,
            const BufferType type,
            const size_t size,
            const size_t count,
            const std::wstring& name) : Buffer{type},device{device} {
        auto minOffsetAlignment = 0;
        if (type == BufferType::UNIFORM) {
            minOffsetAlignment = device->getPhysicalDevice().getDeviceProperties().limits.minUniformBufferOffsetAlignment;
        }
        instanceSizeAligned = minOffsetAlignment > 0
               ? (size + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1)
               : size;
        bufferSize = instanceSizeAligned * count;
        instanceSize = size;
        instanceCount = count;

        const VkBufferCreateFlags usage =
            type == BufferType::VERTEX ? VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT :
            type == BufferType::INDEX ? VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT:
            type == BufferType::INDIRECT ? VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT:
            type == BufferType::STORAGE ? VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT:
            type == BufferType::DEVICE_STORAGE ? VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT:
            type == BufferType::READWRITE_STORAGE ? VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT:
            type == BufferType::IMAGE_UPLOAD ? VK_BUFFER_USAGE_TRANSFER_SRC_BIT:
            type == BufferType::IMAGE_DOWNLOAD ? VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT:
            type == BufferType::BUFFER_UPLOAD ? VK_BUFFER_USAGE_TRANSFER_SRC_BIT:
            type == BufferType::BUFFER_DOWNLOAD ? VK_BUFFER_USAGE_TRANSFER_DST_BIT:
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        const auto memType = (
            type == BufferType::VERTEX ||
            type == BufferType::INDEX ||
            type == BufferType::INDIRECT ||
            type == BufferType::DEVICE_STORAGE ||
            type == BufferType::READWRITE_STORAGE) ?
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT :
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        createBuffer(device, bufferSize, usage, memType, buffer, bufferMemory);
        if constexpr (isMemoryUsageEnabled()) {
            auto lock = std::lock_guard(memoryAllocationsMutex);
            memoryAllocations.push_back({
                VideoMemoryAllocationUsage::BUFFER,
                name,
                bufferSize,
                buffer });
        }
#ifdef _DEBUG
        vkSetObjectName(device->getDevice(), reinterpret_cast<uint64_t>(buffer), VK_OBJECT_TYPE_BUFFER,
            "VKBuffer : " + to_string(name));
        vkSetObjectName(device->getDevice(), reinterpret_cast<uint64_t>(bufferMemory), VK_OBJECT_TYPE_DEVICE_MEMORY,
        "VKBuffer Memory : " + to_string(name));
#endif
    }

    void VKBuffer::map() {
        assert(mappedAddress == nullptr);
        vkMapMemory(device->getDevice(), bufferMemory, 0, bufferSize, 0, &mappedAddress);
    }

    void VKBuffer::unmap() {
        assert(mappedAddress != nullptr);
        vkUnmapMemory(device->getDevice(), bufferMemory);
        mappedAddress = nullptr;
    }

    void VKBuffer::createBuffer(
            const std::shared_ptr<const VKDevice>& device,
            const VkDeviceSize size,
            const VkBufferUsageFlags usage,
            const VkMemoryPropertyFlags memoryTypeIndex,
            VkBuffer& buffer,
            VkDeviceMemory& memory) {
        const auto bufferInfo = VkBufferCreateInfo {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size = size,
            .usage = usage,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        };
        vkCheck(vkCreateBuffer(device->getDevice(), &bufferInfo, nullptr, &buffer));
        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(device->getDevice(), buffer, &memRequirements);
        const auto allocInfo = VkMemoryAllocateInfo {
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .allocationSize = memRequirements.size,
            .memoryTypeIndex = device->getPhysicalDevice().findMemoryType(memRequirements.memoryTypeBits, memoryTypeIndex)
        };
        vkCheck(vkAllocateMemory(device->getDevice(), &allocInfo, nullptr, &memory));
        vkBindBufferMemory(device->getDevice(), buffer, memory, 0);
    }

    VKBuffer::~VKBuffer() {
        if (mappedAddress) {
            VKBuffer::unmap();
        }
        // if constexpr(isMemoryUsageEnabled()) {
            // auto lock = std::lock_guard(memoryAllocationsMutex);
            // memoryAllocations.remove_if([&](const VideoMemoryAllocationDesc& usage) {
                // return usage.ref == buffer;
            // });
        // }
        vkDestroyBuffer(device->getDevice(), buffer, nullptr);
        vkFreeMemory(device->getDevice(), bufferMemory, nullptr);
    }

    VKSampler::VKSampler(
        const std::shared_ptr<const VKDevice>& device,
        const Filter minFilter,
        const Filter magFilter,
        const AddressMode addressModeU,
        const AddressMode addressModeV,
        const AddressMode addressModeW,
        const float minLod,
        const float maxLod,
        const bool anisotropyEnable,
        const MipMapMode mipMapMode) :
        device{device->getDevice()} {
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
            .maxAnisotropy = device->getPhysicalDevice().getDeviceProperties().limits.maxSamplerAnisotropy,
            .compareEnable = VK_FALSE,
            .compareOp = VK_COMPARE_OP_ALWAYS,
            .minLod = minLod,
            .maxLod = maxLod == LOD_CLAMP_NONE ? VK_LOD_CLAMP_NONE : maxLod,
            .borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK,
            .unnormalizedCoordinates = VK_FALSE,
        };
        vkCheck(vkCreateSampler(device->getDevice(), &samplerInfo, nullptr, &sampler));
    }

    VKSampler::~VKSampler() {
        vkDestroySampler(device, sampler, nullptr);
    }

    VKImage::VKImage(
        const std::shared_ptr<const VKDevice>& device,
        const ImageFormat format,
        const uint32_t    width,
        const uint32_t    height,
        const uint32_t    mipLevels,
        const uint32_t    arraySize,
        const std::wstring&    name,
        const bool        useByComputeShader,
        const bool        isRenderTarget,
        const bool        isDepthBuffer,
        const bool        isDepthBufferWithStencil,
        const MSAA        msaa):
        Image{format, width, height, mipLevels, arraySize, useByComputeShader},
        device{device} {
        const VkImageUsageFlags usage =
            isRenderTarget ?
                isDepthBuffer ?
                    msaa != MSAA::NONE ? VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT :
                           VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT
                    : VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT:
            useByComputeShader ? VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT:
            VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        const VkImageCreateFlags flags = arraySize == 6 ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0;
        const auto imageInfo = VkImageCreateInfo {
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .flags = flags,
            .imageType =  VK_IMAGE_TYPE_2D,
            .format = vkFormats[static_cast<int>(format)],
            .extent = {width, height, 1},
            .mipLevels = mipLevels,
            .arrayLayers = arraySize,
            .samples = VKPhysicalDevice::vkSampleCountFlag[static_cast<int>(msaa)],
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .usage = usage,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        };
        vkCheck(vkCreateImage(device->getDevice(), &imageInfo, nullptr, &image));
#ifdef _DEBUG
        vkSetObjectName(device->getDevice(), reinterpret_cast<uint64_t>(image), VK_OBJECT_TYPE_IMAGE,
            to_string((L"VKImage : " + name)));
#endif

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(device->getDevice(), image, &memRequirements);

        const auto allocInfo = VkMemoryAllocateInfo {
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .allocationSize = memRequirements.size,
            .memoryTypeIndex = device->getPhysicalDevice().findMemoryType(
                memRequirements.memoryTypeBits,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
        };
        vkCheck(vkAllocateMemory(device->getDevice(), &allocInfo, nullptr, &imageMemory));
        vkCheck(vkBindImageMemory(device->getDevice(), image, imageMemory, 0));
        if constexpr (isMemoryUsageEnabled()) {
            auto lock = std::lock_guard(memoryAllocationsMutex);
            memoryAllocations.push_back({
                VideoMemoryAllocationUsage::IMAGE,
                name,
                allocInfo.allocationSize,
                image });
        }
#ifdef _DEBUG
        vkSetObjectName(device->getDevice(), reinterpret_cast<uint64_t>(imageMemory), VK_OBJECT_TYPE_DEVICE_MEMORY,
        "VKImage Memory : " + to_string(name));
#endif

        const auto aspect = isDepthBuffer ?
            isDepthBufferWithStencil ?
            VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT:
            VK_IMAGE_ASPECT_DEPTH_BIT :
            VK_IMAGE_ASPECT_COLOR_BIT;
        const auto viewInfo = VkImageViewCreateInfo {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = image,
            .viewType = arraySize == 6 ? VK_IMAGE_VIEW_TYPE_CUBE : VK_IMAGE_VIEW_TYPE_2D,
            .format = vkFormats[static_cast<int>(format)],
            .subresourceRange = {
                .aspectMask = static_cast<uint32_t>(aspect),
                .baseMipLevel = 0,
                .levelCount = mipLevels,
                .baseArrayLayer = 0,
                .layerCount = VK_REMAINING_ARRAY_LAYERS
            }
        };
        vkCheck(vkCreateImageView(device->getDevice(), &viewInfo, nullptr, &imageView));
#ifdef _DEBUG
        vkSetObjectName(device->getDevice(), reinterpret_cast<uint64_t>(imageView), VK_OBJECT_TYPE_IMAGE_VIEW,
            to_string((L"VKImage view : " + name)));
#endif
    }

    VKImage::~VKImage() {
        // if constexpr(isMemoryUsageEnabled()) {
            // auto lock = std::lock_guard(memoryAllocationsMutex);
            // memoryAllocations.remove_if([&](const VideoMemoryAllocationDesc& usage) {
                // return usage.ref == image;
            // });
        // }
        vkDestroyImage(device->getDevice(), image, nullptr);
        vkFreeMemory(device->getDevice(), imageMemory, nullptr);
        vkDestroyImageView(device->getDevice(), imageView, nullptr);
    }


}