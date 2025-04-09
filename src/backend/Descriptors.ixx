/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "Tools.h"
export module vireo.backend.descriptors;

import vireo.backend.buffer;

export namespace vireo::backend {

    enum class DescriptorType {
        BUFFER,
        TEXTURE
    };

    using DescriptorHandle = uint32_t;

    class DescriptorAllocator {
    public:
        DescriptorAllocator(DescriptorType type, uint32_t capacity);

        virtual ~DescriptorAllocator() = default;

        DescriptorHandle allocate();

        void free(DescriptorHandle handle);

        virtual void update(DescriptorHandle handle, Buffer& buffer) = 0;

        virtual uint64_t getGPUHandle(DescriptorHandle handle) const = 0;

    protected:
        DescriptorType                type;
    private:
        const uint32_t                capacity;
        std::vector<DescriptorHandle> freeHandles{};
    };


}