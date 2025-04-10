/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "Tools.h"
export module vireo.backend.descriptors;

import vireo.backend.resources;

export namespace vireo::backend {

    enum class DescriptorType : uint8_t {
        BUFFER  = 0,
        TEXTURE = 1,
    };

    using DescriptorHandle = uint32_t;

    class DescriptorLayout {
    public:
        virtual ~DescriptorLayout() = default;

        auto getCapacity() const { return capacity; }

        auto getType() const { return type; }

    protected:
        DescriptorType                type;
        const size_t                  capacity;

        DescriptorLayout(DescriptorType type, size_t capacity);
    };

    class DescriptorSet {
    public:
        virtual ~DescriptorSet() = default;

        DescriptorHandle allocate();

        void free(DescriptorHandle handle);

        virtual void update(DescriptorHandle handle, Buffer& buffer) = 0;

        virtual void update(std::vector<DescriptorHandle> handles, const std::vector<std::shared_ptr<Buffer>>& buffer) = 0;

        virtual void update(DescriptorHandle handle, Image& buffer) = 0;

    protected:
        const DescriptorLayout&       layout;
        std::vector<DescriptorHandle> freeHandles{};

        DescriptorSet(const DescriptorLayout& layout);
    };


}