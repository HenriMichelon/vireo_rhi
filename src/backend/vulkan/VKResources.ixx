/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "backend/vulkan/Tools.h"
export module vireo.backend.vulkan.resources;

import vireo.backend.resources;
import vireo.backend.vulkan.device;

export namespace vireo::backend {

    class VKBuffer : public Buffer {
    public:
        VKBuffer(
            const VKDevice& device,
            Type type,
            size_t size,
            size_t count,
            size_t minOffsetAlignment,
            const std::wstring& name);

        ~VKBuffer() override;

        void map() override;

        void unmap() override;

        void write(const void* data, size_t size = WHOLE_SIZE, size_t offset = 0) override;

        inline auto getBuffer() const { return buffer; }

        static void createBuffer(
            const VKDevice& device,
            VkDeviceSize size,
            VkBufferUsageFlags usage,
            uint32_t memoryTypeIndex,
            VkBuffer& buffer,
            VkDeviceMemory& memory);

    private:
        const VKDevice& device;
        VkBuffer        buffer{VK_NULL_HANDLE};
        VkDeviceMemory  bufferMemory{VK_NULL_HANDLE};
    };

    class VKSampler : public Sampler {
    public:
        VKSampler(
            const VKPhysicalDevice& physicalDevice,
            VkDevice device,
            Filter minFilter,
            Filter magFilter,
            AddressMode addressModeU,
            AddressMode addressModeV,
            AddressMode addressModeW,
            float minLod = 0.0f,
            float maxLod = 1.0f,
            bool anisotropyEnable = true,
            MipMapMode mipMapMode = MipMapMode::LINEAR);

        ~VKSampler() override;

        auto getSampler() const { return sampler; }

    private:
        VkDevice  device;
        VkSampler sampler;
    };

    class VKImage : public Image {
    public:
        static constexpr  VkFormat vkFormats[] {
            VK_FORMAT_R8G8B8_SRGB,
            VK_FORMAT_R8G8B8A8_SRGB,
        };

        VKImage(
            const VKDevice& device,
            ImageFormat format,
            uint32_t    width,
            uint32_t    height,
            const std::wstring& name);

        ~VKImage() override;

        auto getImage() const { return image; }

        auto getImageView() const { return imageView; }

    private:
        const VKDevice& device;
        VkImage        image{VK_NULL_HANDLE};
        VkDeviceMemory imageMemory{VK_NULL_HANDLE};
        VkImageView    imageView{VK_NULL_HANDLE};
    };

}