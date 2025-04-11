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

    class DXDescriptorLayout : public DescriptorLayout {
    public:
        DXDescriptorLayout(const bool isForSampler) : isForSampler{isForSampler} {}

        DescriptorLayout& add(DescriptorIndex index, DescriptorType type, size_t count = 1) override;

        auto& getRanges() { return ranges; }

        const auto& getStaticSamplesDesc() const { return staticSamplersDesc; }

        auto getIsForSampler() const { return isForSampler; }

    private:
        bool isForSampler;
        std::vector<CD3DX12_DESCRIPTOR_RANGE1> ranges;
        std::vector<D3D12_STATIC_SAMPLER_DESC> staticSamplersDesc;
    };

    class DXDescriptorSet : public DescriptorSet {
    public:
        DXDescriptorSet(const DescriptorLayout& layout, const ComPtr<ID3D12Device>& device, const std::wstring& name);

        ~DXDescriptorSet() override;

        void update(DescriptorIndex index, Buffer& buffer) override;

        void update(DescriptorIndex index, Image& image) override;

        void update(DescriptorIndex index, Sampler& image) override;

        D3D12_GPU_DESCRIPTOR_HANDLE getGPUHandle(DescriptorIndex index) const;

        auto getHeap() const { return heap; }

    private:
        ComPtr<ID3D12Device>         device;
        ComPtr<ID3D12DescriptorHeap> heap;
        D3D12_CPU_DESCRIPTOR_HANDLE  cpuBase;
        D3D12_GPU_DESCRIPTOR_HANDLE  gpuBase;
        uint32_t                     descriptorSize{0};
    };

}