/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/backend/directx/Libraries.h"
export module vireo.directx.resources;

import vireo;

export namespace vireo {

    class DXBuffer : public Buffer {
    public:
        static constexpr D3D12_RESOURCE_STATES resourceStates[] {
            D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,    // VERTEX
            D3D12_RESOURCE_STATE_INDEX_BUFFER,                  // INDEX
            D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT,             // INDIRECT
            D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,    // UNIFORM
            D3D12_RESOURCE_STATE_GENERIC_READ,                  // STORAGE
            D3D12_RESOURCE_STATE_GENERIC_READ,                  // DEVICE_STORAGE
            D3D12_RESOURCE_STATE_UNORDERED_ACCESS,              // READWRITE_STORAGE
            D3D12_RESOURCE_STATE_GENERIC_READ,                  // BUFFER_UPLOAD
            D3D12_RESOURCE_STATE_GENERIC_READ,                  // IMAGE_UPLOAD
            D3D12_RESOURCE_STATE_GENERIC_READ                   // IMAGE_DOWNLOAD
        };

        DXBuffer(
            const ComPtr<ID3D12Device>& device,
            BufferType type,
            size_t size,
            size_t count,
            const std::wstring& name);

        ~DXBuffer() override;

        void map() override;

        void unmap() override;

        auto& getBuffer() const { return buffer; }

        auto getStride() const { return size; }

    private:
        const size_t           size;
        ComPtr<ID3D12Device>   device;
        ComPtr<ID3D12Resource> buffer;
    };

    class DXSampler : public Sampler {
    public:
        static constexpr D3D12_TEXTURE_ADDRESS_MODE addressModes[] {
            D3D12_TEXTURE_ADDRESS_MODE_WRAP,
            D3D12_TEXTURE_ADDRESS_MODE_MIRROR,
            D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
            D3D12_TEXTURE_ADDRESS_MODE_BORDER,
        };

        DXSampler(
            Filter minFilter,
            Filter magFilter,
            AddressMode addressModeU,
            AddressMode addressModeV,
            AddressMode addressModeW,
            float minLod,
            float maxLod,
            bool anisotropyEnable,
            MipMapMode mipMapMode);

        const auto& getSamplerDesc() const { return samplerDesc; }

    private:
        D3D12_SAMPLER_DESC samplerDesc;
    };

    class DXImage : public Image {
    public:
        static constexpr ClearValue defaultClearValue {};
        static constexpr DXGI_FORMAT dxFormats[] = {
            DXGI_FORMAT_R8_UNORM,
            DXGI_FORMAT_R8_SNORM,
            DXGI_FORMAT_R8_UINT,
            DXGI_FORMAT_R8_SINT,

            DXGI_FORMAT_R8G8_UNORM,
            DXGI_FORMAT_R8G8_SNORM,
            DXGI_FORMAT_R8G8_UINT,
            DXGI_FORMAT_R8G8_SINT,

            DXGI_FORMAT_R8G8B8A8_UNORM,
            DXGI_FORMAT_R8G8B8A8_SNORM,
            DXGI_FORMAT_R8G8B8A8_UINT,
            DXGI_FORMAT_R8G8B8A8_SINT,
            DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,

            DXGI_FORMAT_B8G8R8A8_UNORM,
            DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,
            DXGI_FORMAT_B8G8R8X8_UNORM,
            DXGI_FORMAT_B8G8R8X8_UNORM_SRGB,

            DXGI_FORMAT_R10G10B10A2_UNORM,
            DXGI_FORMAT_R10G10B10A2_UINT,

            DXGI_FORMAT_R16_UNORM,
            DXGI_FORMAT_R16_SNORM,
            DXGI_FORMAT_R16_UINT,
            DXGI_FORMAT_R16_SINT,
            DXGI_FORMAT_R16_FLOAT,

            DXGI_FORMAT_R16G16_UNORM,
            DXGI_FORMAT_R16G16_SNORM,
            DXGI_FORMAT_R16G16_UINT,
            DXGI_FORMAT_R16G16_SINT,
            DXGI_FORMAT_R16G16_FLOAT,

            DXGI_FORMAT_R16G16B16A16_UNORM,
            DXGI_FORMAT_R16G16B16A16_SNORM,
            DXGI_FORMAT_R16G16B16A16_UINT,
            DXGI_FORMAT_R16G16B16A16_SINT,
            DXGI_FORMAT_R16G16B16A16_FLOAT,

            DXGI_FORMAT_R32_UINT,
            DXGI_FORMAT_R32_SINT,
            DXGI_FORMAT_R32_FLOAT,

            DXGI_FORMAT_R32G32_UINT,
            DXGI_FORMAT_R32G32_SINT,
            DXGI_FORMAT_R32G32_FLOAT,

            DXGI_FORMAT_R32G32B32_UINT,
            DXGI_FORMAT_R32G32B32_SINT,
            DXGI_FORMAT_R32G32B32_FLOAT,

            DXGI_FORMAT_R32G32B32A32_UINT,
            DXGI_FORMAT_R32G32B32A32_SINT,
            DXGI_FORMAT_R32G32B32A32_FLOAT,

            DXGI_FORMAT_D16_UNORM,
            DXGI_FORMAT_D24_UNORM_S8_UINT,
            DXGI_FORMAT_D32_FLOAT,
            DXGI_FORMAT_D32_FLOAT_S8X24_UINT,

            DXGI_FORMAT_BC1_UNORM,
            DXGI_FORMAT_BC1_UNORM_SRGB,
            DXGI_FORMAT_BC2_UNORM,
            DXGI_FORMAT_BC2_UNORM_SRGB,
            DXGI_FORMAT_BC3_UNORM,
            DXGI_FORMAT_BC3_UNORM_SRGB,
            DXGI_FORMAT_BC4_UNORM,
            DXGI_FORMAT_BC4_SNORM,
            DXGI_FORMAT_BC5_UNORM,
            DXGI_FORMAT_BC5_SNORM,
            DXGI_FORMAT_BC6H_UF16,
            DXGI_FORMAT_BC6H_SF16,
            DXGI_FORMAT_BC7_UNORM,
            DXGI_FORMAT_BC7_UNORM_SRGB,
        };

        DXImage(
            const ComPtr<ID3D12Device> &device,
            ImageFormat format,
            uint32_t    width,
            uint32_t    height,
            uint32_t    mipLevels,
            uint32_t    arraySize,
            const std::wstring& name,
            bool        useByComputeShader,
            bool        isRenderTarget,
            bool        isDepthBuffer,
            ClearValue  clearValue,
            MSAA        msaa);

        auto getImage() const { return image; }

        ~DXImage() override;

    private:
        ComPtr<ID3D12Device>   device;
        ComPtr<ID3D12Resource> image;
    };

    class DXRenderTarget : public RenderTarget {
    public:
        DXRenderTarget(
            const ComPtr<ID3D12Device> &device,
            const std::shared_ptr<DXImage>& image,
            RenderTargetType type);

        auto& getHandle() const { return handle; }

    private:
        ComPtr<ID3D12DescriptorHeap> heap;
        D3D12_CPU_DESCRIPTOR_HANDLE  handle;
    };

}