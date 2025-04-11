/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "backend/directx/Tools.h"
module vireo.backend.directx.descriptors;

import vireo.backend.directx.resources;

namespace vireo::backend {

    DXDescriptorSet::DXDescriptorSet(DescriptorLayout& layout, const ComPtr<ID3D12Device>& device, const std::wstring& name):
        DescriptorSet{layout},
        device{device} {
        const auto heapDesc = D3D12_DESCRIPTOR_HEAP_DESC {
            .Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
            .NumDescriptors = static_cast<UINT>(layout.getCapacity()),
            .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
        };
        DieIfFailed(device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&heap)));
        descriptorSize = device->GetDescriptorHandleIncrementSize(heapDesc.Type);
        cpuBase = heap->GetCPUDescriptorHandleForHeapStart();
        gpuBase = heap->GetGPUDescriptorHandleForHeapStart();
        heap->SetName((L"DXDescriptorSet : " + name).c_str());
    }

    DXDescriptorSet::~DXDescriptorSet() {
        // heap->Release();
    }

    void DXDescriptorSet::update(const DescriptorHandle handle, Buffer& buffer) {
        assert(layout.getType() == DescriptorType::BUFFER);
        const auto dxBuffer = static_cast<DXBuffer&>(buffer);
        const auto bufferViewDesc = D3D12_CONSTANT_BUFFER_VIEW_DESC{
            .BufferLocation = dxBuffer.getBuffer()->GetGPUVirtualAddress(),
            .SizeInBytes = static_cast<UINT>(dxBuffer.getSize())
        };
        const auto descriptorHandle = D3D12_CPU_DESCRIPTOR_HANDLE { cpuBase.ptr + handle * descriptorSize };
        device->CreateConstantBufferView(&bufferViewDesc, descriptorHandle);
    }

    void DXDescriptorSet::update(const DescriptorHandle handle, Image& image) {
        assert(layout.getType() == DescriptorType::IMAGE);
        const auto dxImage = static_cast<DXImage&>(image);
        const auto imageView = D3D12_SHADER_RESOURCE_VIEW_DESC{
            .Format = DXImage::dxFormats[static_cast<int>(dxImage.getFormat())],
            .ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D,
            .Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
            .Texture2D= { .MipLevels = 1 },
        };
        const auto cpuHandle= D3D12_CPU_DESCRIPTOR_HANDLE{ cpuBase.ptr + handle * descriptorSize };
        device->CreateShaderResourceView(dxImage.getImage().Get(), &imageView, cpuHandle);
    }

    D3D12_GPU_DESCRIPTOR_HANDLE DXDescriptorSet::getGPUHandle(const DescriptorHandle handle) const {
        return {gpuBase.ptr + handle * descriptorSize};
    }

}