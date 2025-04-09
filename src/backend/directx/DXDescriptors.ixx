/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "backend/directx/Libraries.h"

export module vireo.backend.directx.descriptors;

import vireo.backend.descriptors;

export namespace vireo::backend {

    class DXDescriptorAllocator : public DescriptorAllocator {
    public:
        DXDescriptorAllocator(DescriptorType type, const ComPtr<ID3D12Device>& device, uint32_t capacity);

        void update(DescriptorHandle handle, void* resource) override;

        uint64_t getGPUHandle(DescriptorHandle handle) const override;

    private:
        ComPtr<ID3D12Device>         device;
        ComPtr<ID3D12DescriptorHeap> heap;
        uint32_t                     descriptorSize{0};
    };

}