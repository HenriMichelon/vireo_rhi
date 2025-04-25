/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/backend/directx/Libraries.h"
module vireo.directx.resources;

import vireo.directx.devices;
import vireo.directx.tools;

namespace vireo {

    DXBuffer::DXBuffer(
        const ComPtr<ID3D12Device>& device,
        const BufferType type,
        const size_t size,
        const size_t count,
        const wstring& name):
        Buffer{type},
        size{size} {
        auto minOffsetAlignment = 0;
        if (type == BufferType::UNIFORM) {
            minOffsetAlignment = 256;
        }
        bufferSize = size * count;
        alignmentSize = minOffsetAlignment > 0
            ? (bufferSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1)
            : bufferSize;
        instanceSize = size;
        instanceCount = count;

        // GPU Buffer
        const auto heapProperties = CD3DX12_HEAP_PROPERTIES(type == BufferType::UNIFORM ? D3D12_HEAP_TYPE_UPLOAD : D3D12_HEAP_TYPE_DEFAULT);
        const auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(alignmentSize);
        dxCheck(device->CreateCommittedResource(
            &heapProperties,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_COMMON,
            nullptr,
            IID_PPV_ARGS(&buffer)));
#ifdef _DEBUG
        buffer->SetName((L"DXBuffer : " + name).c_str());
#endif
    }

    void DXBuffer::map() {
        const CD3DX12_RANGE readRange(0, 0); // We do not intend to read from this resource on the CPU.
        dxCheck(buffer->Map(0, &readRange, &mappedAddress));
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
            const uint32_t    mipLevels,
            const uint32_t    arraySize,
            const wstring&    name,
            const bool        useByComputeShader,
            const bool        isRenderTarget,
            const bool        isDepthBuffer,
            const ClearValue  clearValue,
            const MSAA        msaa):
        Image{format, width, height, mipLevels, arraySize, useByComputeShader} {
        const auto dxFormat = dxFormats[static_cast<int>(format)];
        const auto samples = DXPhysicalDevice::dxSampleCount[static_cast<int>(msaa)];
        UINT quality = 0;
        if (msaa != MSAA::NONE) {
            auto qualityLevels = D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS {
                .Format = dxFormat,
                .SampleCount = samples,
                .Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE,
            };
            dxCheck(device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &qualityLevels, sizeof(qualityLevels)));
            quality = qualityLevels.NumQualityLevels > 0 ? qualityLevels.NumQualityLevels - 1 : 0;
        }
        const auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        const auto imageDesc = D3D12_RESOURCE_DESC{
            .Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
            .Width = width,
            .Height = height,
            .DepthOrArraySize = static_cast<UINT16>(arraySize),
            .MipLevels = 1,
            .Format = dxFormat,
            .SampleDesc = {
                samples,
                quality
            },
            .Flags =
                isRenderTarget ?
                    isDepthBuffer ? D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL :
                    D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET :
                useByComputeShader ? D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS :
                D3D12_RESOURCE_FLAG_NONE,
        };
        auto dxClearValue = D3D12_CLEAR_VALUE{};
        if (isRenderTarget) {
            dxClearValue.Format = imageDesc.Format;
            if (isDepthBuffer) {
                dxClearValue.DepthStencil = {
                    clearValue.depthStencil.depth,
                    static_cast<UINT8>(clearValue.depthStencil.stencil)
                };
            } else {
                dxClearValue.Color[0] = clearValue.color[0];
                dxClearValue.Color[1] = clearValue.color[1];
                dxClearValue.Color[2] = clearValue.color[2];
                dxClearValue.Color[3] = clearValue.color[3];
            }
        }
        dxCheck(device->CreateCommittedResource(
            &heapProperties,
            D3D12_HEAP_FLAG_NONE,
            &imageDesc,
            D3D12_RESOURCE_STATE_COMMON,
            isRenderTarget ? &dxClearValue : nullptr,
            IID_PPV_ARGS(&image)));

#ifdef _DEBUG
        image->SetName((L"DXIMage : " + name).c_str());
#endif
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

    DXRenderTarget::DXRenderTarget(
        const ComPtr<ID3D12Device> &device,
        const shared_ptr<DXImage>& image,
        const RenderTargetType type) :
        RenderTarget{type, image} {
        const auto heapDesc = D3D12_DESCRIPTOR_HEAP_DESC{
            .Type = type == RenderTargetType::COLOR ? D3D12_DESCRIPTOR_HEAP_TYPE_RTV : D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
            .NumDescriptors = 1,
            .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
        };
        dxCheck(device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&heap)));
#ifdef _MSC_VER
        handle = heap->GetCPUDescriptorHandleForHeapStart();
#else
        heap->GetCPUDescriptorHandleForHeapStart(&handle);
#endif
        if (type == RenderTargetType::COLOR) {
            device->CreateRenderTargetView(image->getImage().Get(), nullptr, handle);
        } else {
            device->CreateDepthStencilView(image->getImage().Get(), nullptr, handle);
        }
    }

}
