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
import vireo.backend.samplers;

export namespace vireo::backend {

    enum class DescriptorType : uint8_t {
        BUFFER  = 0,
        TEXTURE = 1,
        SAMPLER = 2,
    };

    using DescriptorHandle = uint32_t;

    class DescriptorSet {
    public:
        DescriptorSet(DescriptorType type, size_t capacity);

        virtual ~DescriptorSet() = default;

        DescriptorHandle allocate();

        void free(DescriptorHandle handle);

        virtual void update(DescriptorHandle handle, Buffer& buffer) = 0;

    protected:
        DescriptorType                type;
    private:
        const size_t                  capacity;
        std::vector<DescriptorHandle> freeHandles{};
    };


}