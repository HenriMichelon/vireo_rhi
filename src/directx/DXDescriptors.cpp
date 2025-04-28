/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/backend/directx/Libraries.h"
module vireo.directx.descriptors;

import std;
import vireo.directx.resources;
import vireo.directx.tools;

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
                type == DescriptorType::BUFFER ? D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC :
                D3D12_DESCRIPTOR_RANGE_FLAG_NONE);
        ranges.push_back(range);
        capacity += count;
        return *this;
    }

    DXDescriptorSet::DXDescriptorSet(const std::shared_ptr<const DescriptorLayout>& layout, const ComPtr<ID3D12Device>& device, const std::wstring& name):
        DescriptorSet{layout},
        device{device} {
        const auto dxLayout = static_pointer_cast<const DXDescriptorLayout>(layout);
        const auto heapDesc = D3D12_DESCRIPTOR_HEAP_DESC {
            .Type = dxLayout->getIsForSampler() ? D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER : D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
            .NumDescriptors = static_cast<UINT>(layout->getCapacity()),
            .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
        };
        dxCheck(device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&heap)));
#ifdef _DEBUG
        heap->SetName((L"DXDescriptorSet : " + name).c_str());
#endif

        descriptorSize = device->GetDescriptorHandleIncrementSize(heapDesc.Type);
#ifdef _MSC_VER
        cpuBase = heap->GetCPUDescriptorHandleForHeapStart();
        gpuBase = heap->GetGPUDescriptorHandleForHeapStart();
#else
        heap->GetCPUDescriptorHandleForHeapStart(&cpuBase);
        heap->GetGPUDescriptorHandleForHeapStart(&gpuBase);
#endif
    }

    DXDescriptorSet::~DXDescriptorSet() {
        // heap->Release();
    }

    void DXDescriptorSet::update(const DescriptorIndex index, const std::shared_ptr<const Buffer>& buffer) const {
        assert(buffer != nullptr);
        const auto dxBuffer = static_pointer_cast<const DXBuffer>(buffer);
        const auto bufferViewDesc = D3D12_CONSTANT_BUFFER_VIEW_DESC{
            .BufferLocation = dxBuffer->getBuffer()->GetGPUVirtualAddress(),
            .SizeInBytes = static_cast<UINT>(dxBuffer->getAlignmentSize()),
        };
        const auto cpuHandle = D3D12_CPU_DESCRIPTOR_HANDLE { cpuBase.ptr + index * descriptorSize };
        device->CreateConstantBufferView(&bufferViewDesc, cpuHandle);
    }

    void DXDescriptorSet::update(const DescriptorIndex index, const std::shared_ptr<const Image>& image) const {
        assert(image != nullptr);
        const auto dxImage = static_pointer_cast<const DXImage>(image);
        const auto cpuHandle= D3D12_CPU_DESCRIPTOR_HANDLE{ cpuBase.ptr + index * descriptorSize };
        if (image->isReadWrite()) {
            const auto viewDesc = D3D12_UNORDERED_ACCESS_VIEW_DESC {
                .Format = DXImage::dxFormats[static_cast<int>(image->getFormat())],
                .ViewDimension = image->getArraySize() > 1 ? D3D12_UAV_DIMENSION_TEXTURE2DARRAY : D3D12_UAV_DIMENSION_TEXTURE2D,
                .Texture2DArray = {
                    .MipSlice = 0,
                    .FirstArraySlice = 0,
                    .ArraySize = image->getArraySize(),
                },
            };
            device->CreateUnorderedAccessView(dxImage->getImage().Get(), nullptr, &viewDesc, cpuHandle);
        } else {
            const auto viewDesc = D3D12_SHADER_RESOURCE_VIEW_DESC {
                .Format = DXImage::dxFormats[static_cast<int>(image->getFormat())],
                .ViewDimension =
                    image->getArraySize() > 1 ?
                        image->getArraySize() == 6 ?
                        D3D12_SRV_DIMENSION_TEXTURECUBE :
                        D3D12_SRV_DIMENSION_TEXTURE2DARRAY :
                    D3D12_SRV_DIMENSION_TEXTURE2D,
                .Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
                .Texture2D= {
                    .MipLevels = 1
                },
            };
            device->CreateShaderResourceView(dxImage->getImage().Get(), &viewDesc, cpuHandle);
        }
    }

    void DXDescriptorSet::update(const DescriptorIndex index, const std::shared_ptr<const Sampler>& sampler) const {
        assert(sampler != nullptr);
        const auto dxSampler = static_pointer_cast<const DXSampler>(sampler);
        const auto samplerDesc = dxSampler->getSamplerDesc();
        const auto cpuHandle= D3D12_CPU_DESCRIPTOR_HANDLE{ cpuBase.ptr + index * descriptorSize };
        device->CreateSampler(&samplerDesc, cpuHandle);
    }

    void DXDescriptorSet::update(const DescriptorIndex index, const std::vector<std::shared_ptr<Buffer>>& buffers) const {
        for (int i = 0; i < buffers.size(); ++i) {
            update(index + i, buffers[i]);
        }
    }

    void DXDescriptorSet::update(const DescriptorIndex index, const std::vector<std::shared_ptr<Image>>& images) const {
        for (int i = 0; i < images.size(); ++i) {
            update(index + i, images[i]);
        }
    }

    void DXDescriptorSet::update(const DescriptorIndex index, const std::vector<std::shared_ptr<Sampler>>& samplers) const {
        for (int i = 0; i < samplers.size(); ++i) {
            update(index + i, samplers[i]);
        }
    }

    D3D12_GPU_DESCRIPTOR_HANDLE DXDescriptorSet::getGPUHandle(const DescriptorIndex index) const {
        return {gpuBase.ptr + index * descriptorSize};
    }

}