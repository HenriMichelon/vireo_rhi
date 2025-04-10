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
import vireo.backend.resources;

export namespace vireo::backend {

    class DXDescriptorSet : public DescriptorSet {
    public:
        DXDescriptorSet(DescriptorType type, const ComPtr<ID3D12Device>& device, size_t capacity, const std::wstring& name);
        ~DXDescriptorSet() override;

        void update(DescriptorHandle handle, Buffer& buffer) override;

        void update(std::vector<DescriptorHandle> handles, const std::vector<std::shared_ptr<Buffer>>& buffer) override {}

        void update(DescriptorHandle handle, Image& buffer) override {}

        D3D12_GPU_DESCRIPTOR_HANDLE getGPUHandle(DescriptorHandle handle) const;

        auto getHeap() const { return heap; }

    private:
        ComPtr<ID3D12Device>         device;
        ComPtr<ID3D12DescriptorHeap> heap;
        D3D12_CPU_DESCRIPTOR_HANDLE  cpuBase;
        D3D12_GPU_DESCRIPTOR_HANDLE  gpuBase;
        uint32_t                     descriptorSize{0};
    };

}