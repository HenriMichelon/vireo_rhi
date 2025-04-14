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
        static constexpr D3D12_RESOURCE_STATES ResourceStates[] {
            D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
            D3D12_RESOURCE_STATE_INDEX_BUFFER,
            D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
        };

        DXBuffer(
            const ComPtr<ID3D12Device>& device,
            BufferType type,
            size_t size,
            size_t count,
            size_t minOffsetAlignment,
            const wstring& name);

        void map() override;

        void unmap() override;

        void write(const void* data, size_t size = WHOLE_SIZE, size_t offset = 0) override;

        const auto& getBufferView() const { return bufferView; }

        const auto& getBufferViewDesc() const { return bufferViewDesc; }

        auto& getBuffer() const { return buffer; }

    private:
        ComPtr<ID3D12Device>            device;
        ComPtr<ID3D12Resource>          buffer;
        D3D12_VERTEX_BUFFER_VIEW        bufferView;
        D3D12_CONSTANT_BUFFER_VIEW_DESC bufferViewDesc;
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
            float minLod = 0.0f,
            float maxLod = 1.0f,
            bool anisotropyEnable = true,
            MipMapMode mipMapMode = MipMapMode::LINEAR);

        const auto& getSamplerDesc() const { return samplerDesc; }

    private:
        D3D12_SAMPLER_DESC samplerDesc;
    };

    class DXImage : public Image {
    public:
        static constexpr DXGI_FORMAT dxFormats[] {
            DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        };

        DXImage(
            const ComPtr<ID3D12Device> &device,
            ImageFormat format,
            uint32_t    width,
            uint32_t    height,
            const wstring& name);

        auto getImage() const { return image; }

        const auto& getImageViewDesc() const { return imageViewDesc; }

    private:
        ComPtr<ID3D12Device>            device;
        ComPtr<ID3D12Resource>          image;
        D3D12_SHADER_RESOURCE_VIEW_DESC imageViewDesc;
    };

}