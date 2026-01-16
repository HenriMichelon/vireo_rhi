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

#if defined(__MINGW32__) || defined(__MINGW64__)
__CRT_UUID_DECL(ID3D12DescriptorHeap,
    0x8efb471d, 0x616c, 0x4f49, 0x90, 0xf7, 0x12, 0x7b, 0xb7, 0x63, 0xfa, 0x51);
#endif

namespace vireo {

    DescriptorLayout& DXDescriptorLayout::add(const DescriptorIndex index, const DescriptorType type, const size_t count) {
        if (isSamplers() && type != DescriptorType::SAMPLER) {
            throw Exception("Sampler descriptor layout only accepts SAMPLER resources");
        }
        if ((!isSamplers()) && type == DescriptorType::SAMPLER) {
            throw Exception("Use Sampler descriptor layout for SAMPLER resources");
        }
        if (isDynamicUniform() && type != DescriptorType::UNIFORM_DYNAMIC) {
            throw Exception("Uniform dynamic descriptor layout only accepts UNIFORM_DYNAMIC resources");
        }
        if ((!isDynamicUniform()) && type == DescriptorType::UNIFORM_DYNAMIC) {
            throw Exception("Use uniform dynamic descriptor layout for UNIFORM_DYNAMIC resources");
        }
        auto unbounded = type == DescriptorType::SAMPLED_IMAGE && count > 1 ;
        CD3DX12_DESCRIPTOR_RANGE1 range;
        range.Init(
                type == DescriptorType::UNIFORM ? D3D12_DESCRIPTOR_RANGE_TYPE_CBV :
                type == DescriptorType::UNIFORM_DYNAMIC ? D3D12_DESCRIPTOR_RANGE_TYPE_CBV :
                type == DescriptorType::SAMPLED_IMAGE ? D3D12_DESCRIPTOR_RANGE_TYPE_SRV :
                type == DescriptorType::DEVICE_STORAGE ? D3D12_DESCRIPTOR_RANGE_TYPE_SRV :
                type == DescriptorType::STORAGE ? D3D12_DESCRIPTOR_RANGE_TYPE_SRV :
                type == DescriptorType::READWRITE_STORAGE ? D3D12_DESCRIPTOR_RANGE_TYPE_UAV :
                type == DescriptorType::READWRITE_IMAGE ? D3D12_DESCRIPTOR_RANGE_TYPE_UAV :
                D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER,
                unbounded ? -1 : count,
                index,
                0, // set when binding
                type == DescriptorType::UNIFORM ? D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC :
                type == DescriptorType::UNIFORM_DYNAMIC ? D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC :
                unbounded ? D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE :
                D3D12_DESCRIPTOR_RANGE_FLAG_NONE);
        ranges.push_back(range);
        capacity += count;
        return *this;
    }

    DXDescriptorHeap::DXDescriptorHeap(
        const ComPtr<ID3D12Device>& device,
        const D3D12_DESCRIPTOR_HEAP_TYPE heapType,
        const uint32_t maxDescriptors):
        device{device},
        maxDescriptors{maxDescriptors} {

        const auto heapDesc = D3D12_DESCRIPTOR_HEAP_DESC{
            .Type = heapType,
            .NumDescriptors = maxDescriptors,
            .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
            .NodeMask = 0
        };

        dxCheck(device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&heap)));
        descriptorSize = device->GetDescriptorHandleIncrementSize(heapDesc.Type);
#if defined(_MSC_VER) || !defined(_WIN32)
        cpuBase = heap->GetCPUDescriptorHandleForHeapStart();
        gpuBase = heap->GetGPUDescriptorHandleForHeapStart();
#else
         heap->GetCPUDescriptorHandleForHeapStart(&cpuBase);
         heap->GetGPUDescriptorHandleForHeapStart(&gpuBase);
#endif
        allocatedDescriptors.resize(maxDescriptors);
    }

    DXDescriptorHeap::DescriptorsArray DXDescriptorHeap::alloc(const uint32_t count) {
        const auto lock = std::lock_guard{mutex};
        for (uint32_t i = 0; i <= maxDescriptors - count; i++) {
            bool available = true;
            for (uint32_t j = 0; j < count; j++) {
                if (allocatedDescriptors[i + j]) {
                    available = false;
                    break;
                }
            }
            if (available) {
                for (uint32_t j = 0; j < count; j++) {
                    allocatedDescriptors[i + j] = true;
                }
                return {
                    .index = i,
                    .count = count,
                    .cpuHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(cpuBase, i, descriptorSize),
                    .gpuHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(gpuBase, i, descriptorSize),
                };
            }
        }
        throw Exception("No contiguous block available in descriptor heap");
    }

    void DXDescriptorHeap::free(const DescriptorsArray& descriptor) {
        const auto lock = std::lock_guard{mutex};
        retiredDescriptors.push_back({descriptor.index, TTL});
    }

    void DXDescriptorHeap::cleanup() {
        const auto lock = std::lock_guard{mutex};
        for (auto& descriptor : retiredDescriptors) {
            if (descriptor.ttl-- == 0) {
                for (int i = 0; i < descriptor.count; ++i) {
                    allocatedDescriptors[descriptor.index + i] = false;
                }
                retiredDescriptors.remove(descriptor);
            }
        }
    }

    DXDescriptorSet::DXDescriptorSet(
        const std::shared_ptr<DXDescriptorHeap>& heap,
        const std::shared_ptr<const DescriptorLayout>& layout,
        const ComPtr<ID3D12Device>& device):
        DescriptorSet{layout},
        heap{heap},
        device{device},
        descriptors{heap->alloc(layout->getCapacity())} {
    }

    void DXDescriptorSet::update(const DescriptorIndex index, const std::shared_ptr<const Buffer>& buffer) {
        assert(!layout->isSamplers());
        assert(buffer != nullptr);
        if (buffer->getType() == BufferType::UNIFORM && layout->isDynamicUniform()) {
            assert(buffer->getType() == BufferType::UNIFORM);
            dynamicBuffer = buffer;
        }
        update(index, *buffer);
    }

        void DXDescriptorSet::update(const DescriptorIndex index, const Buffer& buffer, const Buffer& counterBuffer) {
        assert(!layout->isSamplers());
        assert(buffer.getType() == BufferType::READWRITE_STORAGE || buffer.getType() == BufferType::INDIRECT);
        const auto cpuHandle = D3D12_CPU_DESCRIPTOR_HANDLE { descriptors.cpuHandle.ptr + index * heap->getDescriptorSize() };
        const auto& dxBuffer = static_cast<const DXBuffer&>(buffer);
        const auto& dxCounterBuffer = static_cast<const DXBuffer&>(counterBuffer);
        const auto uavDesc = D3D12_UNORDERED_ACCESS_VIEW_DESC{
            .ViewDimension = D3D12_UAV_DIMENSION_BUFFER,
            .Buffer = {
                .NumElements = buffer.getInstanceCount(),
                .StructureByteStride = buffer.getInstanceSizeAligned(),
            }
        };
        device->CreateUnorderedAccessView(
            dxBuffer.getBuffer().Get(),
            dxCounterBuffer.getBuffer().Get(),
            &uavDesc,
            cpuHandle
        );
    }


    void DXDescriptorSet::update(const DescriptorIndex index, const Buffer& buffer) {
        assert(!layout->isSamplers());
        const auto cpuHandle = D3D12_CPU_DESCRIPTOR_HANDLE { descriptors.cpuHandle.ptr + index * heap->getDescriptorSize() };

        const auto& dxBuffer = static_cast<const DXBuffer&>(buffer);
        if (buffer.getType() == BufferType::UNIFORM) {
            const auto bufferViewDesc = D3D12_CONSTANT_BUFFER_VIEW_DESC{
                .BufferLocation = dxBuffer.getBuffer()->GetGPUVirtualAddress(),
                .SizeInBytes = dxBuffer.getInstanceSizeAligned(),
            };
            device->CreateConstantBufferView(&bufferViewDesc, cpuHandle);
        } else if (buffer.getType() == BufferType::READWRITE_STORAGE ||
                   buffer.getType() == BufferType::INDIRECT) {
            const auto uavDesc = D3D12_UNORDERED_ACCESS_VIEW_DESC{
                .ViewDimension = D3D12_UAV_DIMENSION_BUFFER,
                .Buffer = {
                    .NumElements = buffer.getInstanceCount(),
                    .StructureByteStride = buffer.getInstanceSizeAligned(),
                }
            };
            device->CreateUnorderedAccessView(
                dxBuffer.getBuffer().Get(),
                nullptr,
                &uavDesc,
                cpuHandle
            );
        } else if (
            buffer.getType() == BufferType::STORAGE ||
            buffer.getType() == BufferType::DEVICE_STORAGE) {
            const auto srvDesc = D3D12_SHADER_RESOURCE_VIEW_DESC {
                .Format = DXGI_FORMAT_UNKNOWN,
                .ViewDimension = D3D12_SRV_DIMENSION_BUFFER,
                .Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
                .Buffer = {
                    .FirstElement = 0,
                    .NumElements = buffer.getInstanceCount(),
                    .StructureByteStride = buffer.getInstanceSizeAligned(),
                },
            };
            device->CreateShaderResourceView(
                dxBuffer.getBuffer().Get(),
                &srvDesc,
                cpuHandle
            );
        } else {
            throw Exception("Unsupported buffer type");
        }
    }

    void DXDescriptorSet::update(const DescriptorIndex index, const Image& image) {
        assert(!layout->isDynamicUniform());
        assert(!layout->isSamplers());
        const auto cpuHandle = D3D12_CPU_DESCRIPTOR_HANDLE { descriptors.cpuHandle.ptr + index * heap->getDescriptorSize() };

        const auto& dxImage = static_cast<const DXImage&>(image);
        if (image.isReadWrite()) {
            const auto viewDesc = D3D12_UNORDERED_ACCESS_VIEW_DESC {
                .Format = DXImage::dxFormats[static_cast<int>(image.getFormat())],
                .ViewDimension = image.getArraySize() > 1 ? D3D12_UAV_DIMENSION_TEXTURE2DARRAY : D3D12_UAV_DIMENSION_TEXTURE2D,
                .Texture2DArray = {
                    .MipSlice = 0,
                    .FirstArraySlice = 0,
                    .ArraySize = image.getArraySize(),
                },
            };
            device->CreateUnorderedAccessView(dxImage.getImage().Get(), nullptr, &viewDesc, cpuHandle);
        } else {
            auto viewDesc = D3D12_SHADER_RESOURCE_VIEW_DESC {
                .Format = DXImage::dxFormats[static_cast<int>(image.getFormat())],
                .ViewDimension =
                    image.getArraySize() > 1 ?
                        image.getArraySize() == 6 ?
                        D3D12_SRV_DIMENSION_TEXTURECUBE :
                        D3D12_SRV_DIMENSION_TEXTURE2DARRAY :
                        D3D12_SRV_DIMENSION_TEXTURE2D,
                .Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
                .Texture2D= {
                    .MipLevels = image.getMipLevels()
                },
            };
            if (image.isDepthFormat()) {
                if (image.getFormat() == ImageFormat::D32_SFLOAT) {
                    viewDesc.Format = DXGI_FORMAT_R32_FLOAT;
                } else if (image.getFormat() == ImageFormat::D16_UNORM) {
                    viewDesc.Format = DXGI_FORMAT_R16_FLOAT;
                } else {
                    throw Exception("Unsupported depth format");
                }
            } else if (image.isDepthStencilFormat()) {
                if (image.getFormat() == ImageFormat::D24_UNORM_S8_UINT) {
                    viewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
                } else if (image.getFormat() == ImageFormat::D32_SFLOAT_S8_UINT) {
                        viewDesc.Format = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
                } else {
                    throw Exception("Unsupported depth format");
                }
            }
            device->CreateShaderResourceView(dxImage.getImage().Get(), &viewDesc, cpuHandle);
        }
    }

    void DXDescriptorSet::update(const DescriptorIndex index, const Sampler& sampler) {
        assert(layout->isSamplers());
        const auto cpuHandle = D3D12_CPU_DESCRIPTOR_HANDLE { descriptors.cpuHandle.ptr + index * heap->getDescriptorSize() };

        const auto& dxSampler = static_cast<const DXSampler&>(sampler);
        const auto samplerDesc = dxSampler.getSamplerDesc();
        device->CreateSampler(&samplerDesc, cpuHandle);
    }

    void DXDescriptorSet::update(const DescriptorIndex index, const std::vector<std::shared_ptr<Buffer>>& buffers) {
        for (int i = 0; i < buffers.size(); ++i) {
            update(index + i, buffers[i]);
        }
    }

    void DXDescriptorSet::update(const DescriptorIndex index, const std::vector<std::shared_ptr<Image>>& images) {
        for (int i = 0; i < images.size(); ++i) {
            update(index + i, *images[i]);
        }
    }

    void DXDescriptorSet::update(const DescriptorIndex index, const std::vector<std::shared_ptr<Sampler>>& samplers) {
        for (int i = 0; i < samplers.size(); ++i) {
            update(index + i, *samplers[i]);
        }
    }

}