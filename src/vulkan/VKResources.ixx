/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/backend/vulkan/Libraries.h"
export module vireo.vulkan.resources;

import vireo;

import vireo.vulkan.devices;

export namespace vireo {

    class VKBuffer : public Buffer {
    public:
        VKBuffer(
            const shared_ptr<const VKDevice>& device,
            BufferType type,
            size_t size,
            size_t count,
            size_t minOffsetAlignment,
            const wstring& name);

        ~VKBuffer() override;

        void map() override;

        void unmap() override;

        void write(const void* data, size_t size = WHOLE_SIZE, size_t offset = 0) override;

        inline auto getBuffer() const { return buffer; }

        static void createBuffer(
            const shared_ptr<const VKDevice>& device,
            VkDeviceSize size,
            VkBufferUsageFlags usage,
            uint32_t memoryTypeIndex,
            VkBuffer& buffer,
            VkDeviceMemory& memory);

    private:
        const shared_ptr<const VKDevice> device;
        VkBuffer        buffer{VK_NULL_HANDLE};
        VkDeviceMemory  bufferMemory{VK_NULL_HANDLE};
    };

    class VKSampler : public Sampler {
    public:
        VKSampler(
            const shared_ptr<const VKDevice>& device,
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
        static constexpr VkFormat vkFormats[] = {
            VK_FORMAT_R8_UNORM,
            VK_FORMAT_R8_SNORM,
            VK_FORMAT_R8_UINT,
            VK_FORMAT_R8_SINT,

            VK_FORMAT_R8G8_UNORM,
            VK_FORMAT_R8G8_SNORM,
            VK_FORMAT_R8G8_UINT,
            VK_FORMAT_R8G8_SINT,

            VK_FORMAT_R8G8B8A8_UNORM,
            VK_FORMAT_R8G8B8A8_SNORM,
            VK_FORMAT_R8G8B8A8_UINT,
            VK_FORMAT_R8G8B8A8_SINT,
            VK_FORMAT_R8G8B8A8_SRGB,

            VK_FORMAT_B8G8R8A8_UNORM,
            VK_FORMAT_B8G8R8A8_SRGB,
            VK_FORMAT_B8G8R8A8_UNORM,
            VK_FORMAT_B8G8R8A8_SRGB,

            VK_FORMAT_A2B10G10R10_UNORM_PACK32,
            VK_FORMAT_A2B10G10R10_UINT_PACK32,

            VK_FORMAT_R16_UNORM,
            VK_FORMAT_R16_SNORM,
            VK_FORMAT_R16_UINT,
            VK_FORMAT_R16_SINT,
            VK_FORMAT_R16_SFLOAT,

            VK_FORMAT_R16G16_UNORM,
            VK_FORMAT_R16G16_SNORM,
            VK_FORMAT_R16G16_UINT,
            VK_FORMAT_R16G16_SINT,
            VK_FORMAT_R16G16_SFLOAT,

            VK_FORMAT_R16G16B16A16_UNORM,
            VK_FORMAT_R16G16B16A16_SNORM,
            VK_FORMAT_R16G16B16A16_UINT,
            VK_FORMAT_R16G16B16A16_SINT,
            VK_FORMAT_R16G16B16A16_SFLOAT,

            VK_FORMAT_R32_UINT,
            VK_FORMAT_R32_SINT,
            VK_FORMAT_R32_SFLOAT,

            VK_FORMAT_R32G32_UINT,
            VK_FORMAT_R32G32_SINT,
            VK_FORMAT_R32G32_SFLOAT,

            VK_FORMAT_R32G32B32_UINT,
            VK_FORMAT_R32G32B32_SINT,
            VK_FORMAT_R32G32B32_SFLOAT,

            VK_FORMAT_R32G32B32A32_UINT,
            VK_FORMAT_R32G32B32A32_SINT,
            VK_FORMAT_R32G32B32A32_SFLOAT,

            VK_FORMAT_D16_UNORM,
            VK_FORMAT_D24_UNORM_S8_UINT,
            VK_FORMAT_D32_SFLOAT,
            VK_FORMAT_D32_SFLOAT_S8_UINT,

            VK_FORMAT_BC1_RGB_UNORM_BLOCK,
            VK_FORMAT_BC1_RGB_SRGB_BLOCK,
            VK_FORMAT_BC2_UNORM_BLOCK,
            VK_FORMAT_BC2_SRGB_BLOCK,
            VK_FORMAT_BC3_UNORM_BLOCK,
            VK_FORMAT_BC3_SRGB_BLOCK,
            VK_FORMAT_BC4_UNORM_BLOCK,
            VK_FORMAT_BC4_SNORM_BLOCK,
            VK_FORMAT_BC5_UNORM_BLOCK,
            VK_FORMAT_BC5_SNORM_BLOCK,
            VK_FORMAT_BC6H_UFLOAT_BLOCK,
            VK_FORMAT_BC6H_SFLOAT_BLOCK,
            VK_FORMAT_BC7_UNORM_BLOCK,
            VK_FORMAT_BC7_SRGB_BLOCK,
        };


        VKImage(
            const shared_ptr<const VKDevice>& device,
            ImageFormat format,
            uint32_t    width,
            uint32_t    height,
            const wstring& name,
            bool useByComputeShader,
            bool isRenderTarget,
            bool isDepthBuffer,
            MSAA msaa);

        ~VKImage() override;

        auto getImage() const { return image; }

        auto getImageView() const { return imageView; }

    private:
        const shared_ptr<const VKDevice> device;
        VkImage         image{VK_NULL_HANDLE};
        VkDeviceMemory  imageMemory{VK_NULL_HANDLE};
        VkImageView     imageView{VK_NULL_HANDLE};
    };

}