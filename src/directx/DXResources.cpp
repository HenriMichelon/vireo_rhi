/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/backend/directx/Libraries.h"
module vireo.directx.resources;

import std;
import vireo.directx.devices;
import vireo.directx.pipelines;
import vireo.directx.tools;

namespace vireo {

    DXBuffer::DXBuffer(
        const ComPtr<ID3D12Device>& device,
        const BufferType type,
        const size_t size,
        const size_t count,
        const std::wstring& name):
        Buffer{type},
        size{size} {
        auto minOffsetAlignment = 0;
        if (type == BufferType::UNIFORM) {
            minOffsetAlignment = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT;
        } else if (type == BufferType::IMAGE_UPLOAD || type == BufferType::IMAGE_DOWNLOAD) {
            minOffsetAlignment = D3D12_TEXTURE_DATA_PITCH_ALIGNMENT;
        }
        instanceSizeAligned = minOffsetAlignment > 0
               ? (size + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1)
               : size;
        bufferSize = instanceSizeAligned * count;
        instanceSize = size;
        instanceCount = count;

        const auto heapProperties = CD3DX12_HEAP_PROPERTIES(
            type == BufferType::UNIFORM ||
            type == BufferType::STORAGE ||
            type == BufferType::IMAGE_UPLOAD ||
            type == BufferType::BUFFER_UPLOAD ?
            D3D12_HEAP_TYPE_UPLOAD :
            type == BufferType::IMAGE_DOWNLOAD ||
            type == BufferType::BUFFER_DOWNLOAD ?
            D3D12_HEAP_TYPE_READBACK :
            D3D12_HEAP_TYPE_DEFAULT
        );
        int flag =
            type == BufferType::READWRITE_STORAGE || type == BufferType::INDIRECT ?
            D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS :
            D3D12_RESOURCE_FLAG_NONE;
        const auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize, static_cast<D3D12_RESOURCE_FLAGS >(flag));
        dxCheck(device->CreateCommittedResource(
            &heapProperties,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_COMMON,
            nullptr,
            IID_PPV_ARGS(&buffer)));
        if constexpr (isMemoryUsageEnabled()) {
            auto lock = std::lock_guard(memoryAllocationsMutex);
            memoryAllocations.push_back({
                VideoMemoryAllocationUsage::BUFFER,
                name,
                bufferSize,
                buffer.Get() });
        }
#ifdef _DEBUG
        buffer->SetName((L"DXBuffer : " + name).c_str());
#endif
    }

     DXBuffer::~DXBuffer() {
        if (mappedAddress) {
            DXBuffer::unmap();
        }
        // if constexpr(isMemoryUsageEnabled()) {
            // auto lock = std::lock_guard(memoryAllocationsMutex);
            // memoryAllocations.remove_if([&](const VideoMemoryAllocationDesc& usage) {
                // return usage.ref == buffer.Get();
            // });
        // }
    }

    void DXBuffer::map() {
        assert(mappedAddress == nullptr);
        const CD3DX12_RANGE readRange(0, 0); // We do not intend to read from this resource on the CPU.
        dxCheck(buffer->Map(0, &readRange, &mappedAddress));
    }

    void DXBuffer::unmap() {
        assert(mappedAddress != nullptr);
        buffer->Unmap(0, nullptr);
        mappedAddress = nullptr;
    }

    DXImage::DXImage(
            const ComPtr<ID3D12Device> &device,
            const ImageFormat format,
            const uint32_t    width,
            const uint32_t    height,
            const uint32_t    mipLevels,
            const uint32_t    arraySize,
            const std::wstring&    name,
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
            .MipLevels = static_cast<UINT16>(mipLevels),
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

        if constexpr (isMemoryUsageEnabled()) {
            const auto allocInfo = device->GetResourceAllocationInfo(
                0,1, &imageDesc);
            auto lock = std::lock_guard(memoryAllocationsMutex);
            memoryAllocations.push_back({
                VideoMemoryAllocationUsage::IMAGE,
                name,
                allocInfo.SizeInBytes,
                image.Get() });
        }
#ifdef _DEBUG
        image->SetName((L"DXIMage : " + name).c_str());
#endif
    }

    DXImage::~DXImage() {
        // if constexpr(isMemoryUsageEnabled()) {
            // auto lock = std::lock_guard(memoryAllocationsMutex);
            // memoryAllocations.remove_if([&](const VideoMemoryAllocationDesc& usage) {
                // return usage.ref == image.Get();
            // });
        // }
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
        const MipMapMode mipMapMode,
        CompareOp compareOp) {

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
            .MaxAnisotropy = D3D12_REQ_MAXANISOTROPY ,
            .ComparisonFunc = DXGraphicPipeline::dxCompareOp[static_cast<int>(compareOp)],
            .BorderColor = {0.0f, 0.0f, 0.0f, 0.0f},
            .MinLOD = minLod,
            .MaxLOD = maxLod == LOD_CLAMP_NONE ? D3D12_FLOAT32_MAX : maxLod,
        };
    }

    DXRenderTarget::DXRenderTarget(
        const ComPtr<ID3D12Device> &device,
        const std::shared_ptr<DXImage>& image,
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
