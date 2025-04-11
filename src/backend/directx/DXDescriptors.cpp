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

    DescriptorLayout& DXDescriptorLayout::add(const DescriptorIndex index, const std::vector<std::shared_ptr<Sampler>>& staticSamplers) {
        this->staticSamplersDesc.resize(staticSamplers.size());
        for (int i = 0; i < staticSamplers.size(); i++) {
            this->staticSamplersDesc[i] = static_pointer_cast<DXSampler>(staticSamplers[i])->getSamplerDesc();
            staticSamplersDesc[i].ShaderRegister = index+i;
            staticSamplersDesc[i].RegisterSpace = 0;
            assert(staticSamplersDesc[i].Filter <= D3D12_FILTER_ANISOTROPIC);
            assert(staticSamplersDesc[i].MinLOD <= staticSamplersDesc[i].MaxLOD);
        }
        return *this;
    }

    DescriptorLayout& DXDescriptorLayout::add(const DescriptorIndex index, const DescriptorType type, const size_t count) {
        CD3DX12_DESCRIPTOR_RANGE1 range;
        range.Init(
                type == DescriptorType::BUFFER ? D3D12_DESCRIPTOR_RANGE_TYPE_CBV : D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
                count,
                index,
                0,
                D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
        ranges.push_back(range);
        capacity += count;
        return *this;
    }

    DXDescriptorSet::DXDescriptorSet(const DescriptorLayout& layout, const ComPtr<ID3D12Device>& device, const std::wstring& name):
        DescriptorSet{layout},
        device{device} {
        const auto heapDesc = D3D12_DESCRIPTOR_HEAP_DESC {
            .Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
            .NumDescriptors = static_cast<UINT>(layout.getCapacity()),
            .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
        };
        DieIfFailed(device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&heap)));
        heap->SetName((L"DXDescriptorSet : " + name).c_str());

        descriptorSize = device->GetDescriptorHandleIncrementSize(heapDesc.Type);
        cpuBase = heap->GetCPUDescriptorHandleForHeapStart();
        gpuBase = heap->GetGPUDescriptorHandleForHeapStart();
    }

    DXDescriptorSet::~DXDescriptorSet() {
        // heap->Release();
    }

    void DXDescriptorSet::update(const DescriptorIndex index, Buffer& buffer) {
        const auto bufferViewDesc = static_cast<DXBuffer&>(buffer).getBufferViewDesc();
        const auto cpuHandle = D3D12_CPU_DESCRIPTOR_HANDLE { cpuBase.ptr + index * descriptorSize };
        device->CreateConstantBufferView(&bufferViewDesc, cpuHandle);
    }

    void DXDescriptorSet::update(const DescriptorIndex index, Image& image) {
        const auto dxImage = static_cast<DXImage&>(image);
        const auto cpuHandle= D3D12_CPU_DESCRIPTOR_HANDLE{ cpuBase.ptr + index * descriptorSize };
        device->CreateShaderResourceView(dxImage.getImage().Get(), &dxImage.getImageViewDesc(), cpuHandle);
    }

    D3D12_GPU_DESCRIPTOR_HANDLE DXDescriptorSet::getGPUHandle(const DescriptorIndex index) const {
        return {gpuBase.ptr + index * descriptorSize};
    }

}