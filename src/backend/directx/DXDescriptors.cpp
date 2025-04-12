/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/backend/directx/Tools.h"
module vireo.backend.directx.descriptors;

import vireo.backend.directx.resources;

namespace vireo::backend {

    DescriptorLayout& DXDescriptorLayout::add(const DescriptorIndex index, const DescriptorType type, const size_t count) {
        CD3DX12_DESCRIPTOR_RANGE1 range;
        range.Init(
                type == DescriptorType::BUFFER ? D3D12_DESCRIPTOR_RANGE_TYPE_CBV :
                    type == DescriptorType::IMAGE ? D3D12_DESCRIPTOR_RANGE_TYPE_SRV :
                    D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER,
                count,
                index,
                0, // set when binding
                type == DescriptorType::SAMPLER ? D3D12_DESCRIPTOR_RANGE_FLAG_NONE : D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
        ranges.push_back(range);
        capacity += count;
        return *this;
    }

    DXDescriptorSet::DXDescriptorSet(const shared_ptr<DescriptorLayout>& layout, const ComPtr<ID3D12Device>& device, const wstring& name):
        DescriptorSet{layout},
        device{device} {
        const auto dxLayout = static_pointer_cast<const DXDescriptorLayout>(layout);
        const auto heapDesc = D3D12_DESCRIPTOR_HEAP_DESC {
            .Type = dxLayout->getIsForSampler() ? D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER : D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
            .NumDescriptors = static_cast<UINT>(layout->getCapacity()),
            .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
        };
        DieIfFailed(device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&heap)));
#ifdef _DEBUG
        heap->SetName((L"DXDescriptorSet : " + name).c_str());
#endif

        descriptorSize = device->GetDescriptorHandleIncrementSize(heapDesc.Type);
        cpuBase = heap->GetCPUDescriptorHandleForHeapStart();
        gpuBase = heap->GetGPUDescriptorHandleForHeapStart();
    }

    DXDescriptorSet::~DXDescriptorSet() {
        // heap->Release();
    }

    void DXDescriptorSet::update(const DescriptorIndex index, const shared_ptr<Buffer>& buffer) {
        const auto bufferViewDesc = static_pointer_cast<DXBuffer>(buffer)->getBufferViewDesc();
        const auto cpuHandle = D3D12_CPU_DESCRIPTOR_HANDLE { cpuBase.ptr + index * descriptorSize };
        device->CreateConstantBufferView(&bufferViewDesc, cpuHandle);
    }

    void DXDescriptorSet::update(const DescriptorIndex index, const shared_ptr<Image>& image) {
        const auto dxImage = static_pointer_cast<DXImage>(image);
        const auto cpuHandle= D3D12_CPU_DESCRIPTOR_HANDLE{ cpuBase.ptr + index * descriptorSize };
        device->CreateShaderResourceView(dxImage->getImage().Get(), &dxImage->getImageViewDesc(), cpuHandle);
    }

    void DXDescriptorSet::update(const DescriptorIndex index, const shared_ptr<Sampler>& sampler) {
        auto dxSampler = static_pointer_cast<DXSampler>(sampler);
        auto samplerDesc = dxSampler->getSamplerDesc();
        const auto cpuHandle= D3D12_CPU_DESCRIPTOR_HANDLE{ cpuBase.ptr + index * descriptorSize };
        device->CreateSampler(&samplerDesc, cpuHandle);
    }

    void DXDescriptorSet::update(const DescriptorIndex index, const vector<shared_ptr<Buffer>>& buffers) {
        for (int i = 0; i < buffers.size(); ++i) {
            update(index + i, buffers[i]);
        }
    }

    void DXDescriptorSet::update(const DescriptorIndex index, const vector<shared_ptr<Image>>& images) {
        for (int i = 0; i < images.size(); ++i) {
            update(index + i, images[i]);
        }
    }

    void DXDescriptorSet::update(const DescriptorIndex index, const vector<shared_ptr<Sampler>>& samplers) {
        for (int i = 0; i < samplers.size(); ++i) {
            update(index + i, samplers[i]);
        }
    }

    D3D12_GPU_DESCRIPTOR_HANDLE DXDescriptorSet::getGPUHandle(const DescriptorIndex index) const {
        return {gpuBase.ptr + index * descriptorSize};
    }

}