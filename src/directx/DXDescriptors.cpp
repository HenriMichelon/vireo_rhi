/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/backend/directx/Tools.h"
module vireo.directx.descriptors;

import vireo.directx.resources;

namespace vireo {

    DescriptorLayout& DXDescriptorLayout::add(const DescriptorIndex index, const DescriptorType type, const size_t count) {
        CD3DX12_DESCRIPTOR_RANGE1 range;
        range.Init(
                type == DescriptorType::BUFFER ? D3D12_DESCRIPTOR_RANGE_TYPE_CBV :
                type == DescriptorType::SAMPLED_IMAGE ? D3D12_DESCRIPTOR_RANGE_TYPE_SRV :
                type == DescriptorType::READWRITE_IMAGE ? D3D12_DESCRIPTOR_RANGE_TYPE_UAV :
                D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER,
                count,
                index,
                0, // set when binding
                type == DescriptorType::SAMPLER ? D3D12_DESCRIPTOR_RANGE_FLAG_NONE : D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
        ranges.push_back(range);
        capacity += count;
        return *this;
    }

    DXDescriptorSet::DXDescriptorSet(const shared_ptr<const DescriptorLayout>& layout, const ComPtr<ID3D12Device>& device, const wstring& name):
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

    void DXDescriptorSet::update(const DescriptorIndex index, const shared_ptr<const Buffer>& buffer) const {
        const auto bufferViewDesc = static_pointer_cast<const DXBuffer>(buffer)->getBufferViewDesc();
        const auto cpuHandle = D3D12_CPU_DESCRIPTOR_HANDLE { cpuBase.ptr + index * descriptorSize };
        device->CreateConstantBufferView(&bufferViewDesc, cpuHandle);
    }

    void DXDescriptorSet::update(const DescriptorIndex index, const shared_ptr<const Image>& image, bool useByComputeShader) const {
        const auto dxImage = static_pointer_cast<const DXImage>(image);
        const auto cpuHandle= D3D12_CPU_DESCRIPTOR_HANDLE{ cpuBase.ptr + index * descriptorSize };
        device->CreateShaderResourceView(dxImage->getImage().Get(), &dxImage->getImageViewDesc(), cpuHandle);
    }

    void DXDescriptorSet::update(const DescriptorIndex index, const shared_ptr<const Sampler>& sampler) const {
        const auto dxSampler = static_pointer_cast<const DXSampler>(sampler);
        const auto samplerDesc = dxSampler->getSamplerDesc();
        const auto cpuHandle= D3D12_CPU_DESCRIPTOR_HANDLE{ cpuBase.ptr + index * descriptorSize };
        device->CreateSampler(&samplerDesc, cpuHandle);
    }

    void DXDescriptorSet::update(const DescriptorIndex index, const vector<shared_ptr<Buffer>>& buffers) const {
        for (int i = 0; i < buffers.size(); ++i) {
            update(index + i, buffers[i]);
        }
    }

    void DXDescriptorSet::update(const DescriptorIndex index, const vector<shared_ptr<Image>>& images, bool useByComputeShader) const {
        for (int i = 0; i < images.size(); ++i) {
            update(index + i, images[i], useByComputeShader);
        }
    }

    void DXDescriptorSet::update(const DescriptorIndex index, const vector<shared_ptr<Sampler>>& samplers) const {
        for (int i = 0; i < samplers.size(); ++i) {
            update(index + i, samplers[i]);
        }
    }

    D3D12_GPU_DESCRIPTOR_HANDLE DXDescriptorSet::getGPUHandle(const DescriptorIndex index) const {
        return {gpuBase.ptr + index * descriptorSize};
    }

}