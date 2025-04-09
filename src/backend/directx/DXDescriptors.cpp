/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "backend/directx/Tools.h"

module vireo.backend.directx.descriptors;

namespace vireo::backend {

    DXDescriptorAllocator::DXDescriptorAllocator(const DescriptorType type, const ComPtr<ID3D12Device>& device, uint32_t capacity):
        DescriptorAllocator{type, capacity},
        device{device} {
        const auto heapDesc = D3D12_DESCRIPTOR_HEAP_DESC {
            .Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
            .NumDescriptors = capacity,
            .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
        };

        //device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_heap));

        //descriptorSize = device->GetDescriptorHandleIncrementSize(desc.Type);
    }


    void DXDescriptorAllocator::update(DescriptorHandle handle, void* resource) {

    }

    uint64_t DXDescriptorAllocator::getGPUHandle(DescriptorHandle handle) const {
        return 0;
    }

}