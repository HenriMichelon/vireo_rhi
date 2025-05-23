/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/backend/directx/Libraries.h"
export module vireo.directx.descriptors;

import std;
import vireo;

export namespace vireo {

    class DXDescriptorLayout : public DescriptorLayout {
    public:
        DXDescriptorLayout(const bool isForSampler, const bool isDynamic) : DescriptorLayout{isForSampler, isDynamic} {}

        DescriptorLayout& add(DescriptorIndex index, DescriptorType type, size_t count = 1) override;

        auto& getRanges() { return ranges; }

        const auto& getStaticSamplesDesc() const { return staticSamplersDesc; }

    private:
        std::vector<CD3DX12_DESCRIPTOR_RANGE1> ranges;
        std::vector<D3D12_STATIC_SAMPLER_DESC> staticSamplersDesc;
    };

    class DXDescriptorHeap  {
    public:
        struct DescriptorsArray {
            uint32_t                    index;
            uint32_t                    count;
            D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
            D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle;
        };
        struct RetiredDescriptor {
            uint32_t index;
            uint32_t count;
            uint32_t ttl;

            friend bool operator==(const RetiredDescriptor& lhs, const RetiredDescriptor& rhs) {
                return lhs.index == rhs.index;
            }
        };

        DXDescriptorHeap(const ComPtr<ID3D12Device>& device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, uint32_t maxDescriptors);

        DescriptorsArray alloc(uint32_t count);

        void free(const DescriptorsArray& descriptor);

        void cleanup();

        auto getDescriptorSize() const { return descriptorSize; }

        auto getHeap() const { return heap; }

    private:
        static constexpr uint32_t    TTL{3};
        ComPtr<ID3D12Device>         device;
        ComPtr<ID3D12DescriptorHeap> heap;
        uint32_t                     maxDescriptors;
        uint32_t                     descriptorSize;
        D3D12_CPU_DESCRIPTOR_HANDLE  cpuBase;
        D3D12_GPU_DESCRIPTOR_HANDLE  gpuBase;
        std::vector<bool>            allocatedDescriptors;
        std::list<RetiredDescriptor> retiredDescriptors;
        std::mutex                   mutex;
    };

    class DXDescriptorSet : public DescriptorSet {
    public:
        DXDescriptorSet(
            const std::shared_ptr<DXDescriptorHeap>& heap,
            const std::shared_ptr<const DescriptorLayout>& layout,
            const ComPtr<ID3D12Device>& device);

        void update(DescriptorIndex index, const std::shared_ptr<const Buffer>& buffer) override;

        void update(DescriptorIndex index, const Buffer& buffer) override;

        void update(DescriptorIndex index, const Image& image) override;

        void update(DescriptorIndex index, const Sampler& sampler) override;

        void update(DescriptorIndex index, const std::vector<std::shared_ptr<Buffer>>& buffers) override;

        void update(DescriptorIndex index, const std::vector<std::shared_ptr<Image>>& images) override;

        void update(DescriptorIndex index, const std::vector<std::shared_ptr<Sampler>>& samplers) override;

        const auto& getDynamicBuffer() const { return dynamicBuffer; }

        const auto& getDescriptors() const { return descriptors; }

    private:
        std::shared_ptr<DXDescriptorHeap>  heap;
        ComPtr<ID3D12Device>               device;
        std::shared_ptr<const Buffer>      dynamicBuffer{nullptr};
        DXDescriptorHeap::DescriptorsArray descriptors;
    };

}