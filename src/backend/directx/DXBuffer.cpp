/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "backend/directx/Tools.h"
module vireo.backend.directx.buffer;

namespace vireo::backend {

    DXBuffer::DXBuffer(
        const ComPtr<ID3D12Device>& device,
        const Type type,
        const size_t size,
        const size_t count,
        size_t minOffsetAlignment,
        const std::wstring& name):
        Buffer{type} {
        alignmentSize = minOffsetAlignment > 0
            ? (size + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1)
            : size;
        bufferSize = alignmentSize * count;

        // GPU Buffer
        const auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
        DieIfFailed(device->CreateCommittedResource(
            &heapProperties,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_COMMON,
            nullptr,
            IID_PPV_ARGS(&buffer)));
        buffer->SetName(name.c_str());
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
            memcpy(reinterpret_cast<unsigned char*>(mappedAddress) + offset, data, size);
        }
    }

}
