/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include <cstring>
#include <cassert>
module vireo;

import std;
import vireo.tools;
import vireo.vulkan;
#ifdef DIRECTX_BACKEND
import vireo.directx;
#endif

namespace vireo {

    std::mutex Buffer::memoryAllocationsMutex;
    std::list<VideoMemoryAllocationDesc> Buffer::memoryAllocations{};

    std::mutex Image::memoryAllocationsMutex;
    std::list<VideoMemoryAllocationDesc> Image::memoryAllocations{};

    std::shared_ptr<Vireo> Vireo::create(
        const Backend backend,
        const uint32_t maxDirectX12Descriptors,
        const uint32_t maxDirectX12Samplers) {
        if (backend == Backend::VULKAN) {
            return std::make_shared<VKVireo>();
        }
#ifdef DIRECTX_BACKEND
        return std::make_shared<DXVireo>(maxDirectX12Descriptors, maxDirectX12Samplers);
#endif
        throw Exception("Unsupported backend");
    }

    bool Vireo::isBackendSupported(const Backend backend) {
#ifdef DIRECTX_BACKEND
        if (backend == Backend::DIRECTX) { return true; }
#endif
        return backend == Backend::VULKAN;
    }

    std::shared_ptr<DescriptorLayout> Vireo::createDynamicUniformDescriptorLayout(
            const std::wstring& name) const {
        const auto layout = _createDynamicUniformDescriptorLayout(name);
        layout->add(0, DescriptorType::UNIFORM_DYNAMIC);
        layout->build();
        return layout;
    }

    void Buffer::write(const void* data, const size_t size, const size_t offset) const {
        assert(mappedAddress != nullptr);
        assert(data != nullptr);
        assert(type == BufferType::UNIFORM ||
               type == BufferType::STORAGE ||
               type == BufferType::IMAGE_UPLOAD ||
               type == BufferType::BUFFER_UPLOAD);
        if (size == WHOLE_SIZE) {
            for (int y = 0; y < instanceCount; y++) {
                auto *pScan = static_cast<uint8_t*>(mappedAddress) + y * instanceSizeAligned;
                auto *pSource = static_cast<const uint8_t*>(data) + y * instanceSize;
                memcpy(pScan, pSource, instanceSize);
            }
        } else {
            assert((offset + size) <= bufferSize);
            memcpy(static_cast<unsigned char*>(mappedAddress) + offset, data, size);
        }
    }

    void CommandList::upload(const std::vector<BufferUploadInfo>& infos) {
        for (const auto &info : infos) {
            upload(info.buffer, info.data);
        }
    }

    void CommandList::upload(const std::vector<ImageUploadInfo>& infos) {
        for (const auto &info : infos) {
            upload(info.image, info.data);
        }
    }

    uint32_t Image::getRowPitch(const uint32_t mipLevel) const {
        if (format >= ImageFormat::BC1_UNORM) {
            return (((width >> mipLevel) + 3) / 4) * pixelSize[static_cast<int>(format)];
        }
        return (width >> mipLevel) * pixelSize[static_cast<int>(format)];
    }

    uint32_t Image::getRowLength(const uint32_t mipLevel) const {
        if (format >= ImageFormat::BC1_UNORM) {
            return ((width >> mipLevel) + 3) & ~3;
        }
        return (width >> mipLevel) ;
    }

}