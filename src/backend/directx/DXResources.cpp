/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/backend/directx/Tools.h"
module vireo.backend.directx.resources;

namespace vireo::backend {

    DXBuffer::DXBuffer(
        const ComPtr<ID3D12Device>& device,
        const Type type,
        const size_t size,
        const size_t count,
        const size_t minOffsetAlignment,
        const std::wstring& name):
        Buffer{type} {
        alignmentSize = minOffsetAlignment > 0
            ? (size + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1)
            : size;
        bufferSize = alignmentSize * count;

        // GPU Buffer
        const auto heapProperties = CD3DX12_HEAP_PROPERTIES(type == UNIFORM ? D3D12_HEAP_TYPE_UPLOAD : D3D12_HEAP_TYPE_DEFAULT);
        const auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
        DieIfFailed(device->CreateCommittedResource(
            &heapProperties,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            type == UNIFORM ? D3D12_RESOURCE_STATE_GENERIC_READ : D3D12_RESOURCE_STATE_COMMON,
            nullptr,
            IID_PPV_ARGS(&buffer)));
#ifdef _DEBUG
        buffer->SetName((L"DXBuffer : " + name).c_str());
#endif

        bufferViewDesc.BufferLocation = buffer->GetGPUVirtualAddress();
        bufferViewDesc.SizeInBytes = static_cast<UINT>(bufferSize);

        bufferView.BufferLocation = buffer->GetGPUVirtualAddress();
        bufferView.StrideInBytes = size;
        bufferView.SizeInBytes = bufferSize;
    }

    void DXBuffer::map() {
        CD3DX12_RANGE readRange(0, 0); // We do not intend to read from this resource on the CPU.
        DieIfFailed(buffer->Map(0, &readRange, &mappedAddress));
    }

    void DXBuffer::unmap() {
        buffer->Unmap(0, nullptr);
        mappedAddress = nullptr;
    }

    void DXBuffer::write(const void* data, const size_t size, const size_t offset) {
        if (size == WHOLE_SIZE) {
            memcpy(mappedAddress, data, bufferSize);
        } else {
            memcpy(static_cast<unsigned char*>(mappedAddress) + offset, data, size);
        }
    }

    DXImage::DXImage(
            const ComPtr<ID3D12Device> &device,
            const ImageFormat format,
            const uint32_t    width,
            const uint32_t    height,
            const std::wstring& name):
        Image{format, width, height} {
        const auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        const auto imageDesc = D3D12_RESOURCE_DESC{
            .Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
            .Width = width,
            .Height = height,
            .DepthOrArraySize = 1,
            .MipLevels = 1,
            .Format = dxFormats[static_cast<int>(format)],
            .SampleDesc = { 1, 0 },
            .Flags = D3D12_RESOURCE_FLAG_NONE,
        };
        DieIfFailed(device->CreateCommittedResource(
            &heapProperties,
            D3D12_HEAP_FLAG_NONE,
            &imageDesc,
            D3D12_RESOURCE_STATE_COMMON,
            nullptr,
            IID_PPV_ARGS(&image)));

#ifdef _DEBUG
        image->SetName((L"DXIMage : " + name).c_str());
#endif

        imageViewDesc.Format = imageDesc.Format;
        imageViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        imageViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        imageViewDesc.Texture2D= { .MipLevels = 1 };
    }

    DXSampler::DXSampler(
        const Filter minFilter,
        const Filter magFilter,
        const AddressMode addressModeU,
        const AddressMode addressModeV,
        const AddressMode addressModeW,
        const float minLod,
        const float maxLod,
        const bool anisotropyEnable,
        const MipMapMode mipMapMode) {

        int filter = anisotropyEnable ? D3D12_FILTER_ANISOTROPIC : 0;
        if ((minFilter == Filter::LINEAR) && (magFilter == Filter::LINEAR)) {
            filter |= mipMapMode == MipMapMode::LINEAR ? D3D12_FILTER_MIN_MAG_MIP_LINEAR : D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
        } else if ((minFilter == Filter::NEAREST) && (magFilter == Filter::NEAREST)) {
            filter |= mipMapMode == MipMapMode::LINEAR ? D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR : D3D12_FILTER_MIN_MAG_MIP_POINT;
        } else if ((minFilter == Filter::NEAREST) && (magFilter == Filter::LINEAR)) {
            filter |= mipMapMode == MipMapMode::LINEAR ? D3D12_FILTER_MIN_POINT_MAG_MIP_LINEAR : D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
        } else if ((minFilter == Filter::LINEAR) && (magFilter == Filter::NEAREST)) {
            filter |= mipMapMode == MipMapMode::LINEAR ? D3D12_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR : D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT;
        }

        samplerDesc = D3D12_SAMPLER_DESC{
            .Filter = static_cast<D3D12_FILTER>(filter),
            .AddressU = addressModes[static_cast<int>(addressModeU)],
            .AddressV = addressModes[static_cast<int>(addressModeV)],
            .AddressW = addressModes[static_cast<int>(addressModeW)],
            .MipLODBias = 0,
            .MaxAnisotropy = 0,
            .ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER,
            .BorderColor = {0.0f, 0.0f, 0.0f, 0.0f},
            .MinLOD = minLod,
            .MaxLOD = maxLod,
        };
    }

}
