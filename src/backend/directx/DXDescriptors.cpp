/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "backend/directx/Tools.h"
module vireo.backend.directx.descriptors;

import vireo.backend.directx.buffer;

namespace vireo::backend {

    DXDescriptorSet::DXDescriptorSet(const DescriptorType type, const ComPtr<ID3D12Device>& device, size_t capacity):
        DescriptorSet{type, capacity},
        device{device} {
        const auto heapDesc = D3D12_DESCRIPTOR_HEAP_DESC {
            .Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
            .NumDescriptors = static_cast<UINT>(capacity),
            .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
        };
        DieIfFailed(device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&heap)));
        descriptorSize = device->GetDescriptorHandleIncrementSize(heapDesc.Type);
        cpuBase = heap->GetCPUDescriptorHandleForHeapStart();
        gpuBase = heap->GetGPUDescriptorHandleForHeapStart();
    }

    DXDescriptorSet::~DXDescriptorSet() {
        // heap->Release();
    }

    void DXDescriptorSet::update(const DescriptorHandle handle, Buffer& buffer) {
        assert(type == DescriptorType::BUFFER);
        const auto dxBuffer = static_cast<DXBuffer&>(buffer);
        const auto bufferViewDesc = D3D12_CONSTANT_BUFFER_VIEW_DESC{
            .BufferLocation = dxBuffer.getBuffer()->GetGPUVirtualAddress(),
            .SizeInBytes = static_cast<UINT>(dxBuffer.getSize())
        };
        const auto descriptorHandle = D3D12_CPU_DESCRIPTOR_HANDLE { cpuBase.ptr + handle * descriptorSize };
        device->CreateConstantBufferView(&bufferViewDesc, descriptorHandle);
    }

    uint64_t DXDescriptorSet::getGPUHandle(const DescriptorHandle handle) const {
        return gpuBase.ptr + handle * descriptorSize;
    }

}