/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "Tools.h"

export module vireo.backend.buffer;

export namespace vireo::backend {

    class Buffer {
    public:
        enum Type {
            VERTEX,
            INDEX,
            UNIFORM
        };

        static constexpr size_t WHOLE_SIZE = ~0ULL;

        Buffer(const Type type): type{type} {}

        virtual ~Buffer() = default;

        auto getSize() const { return bufferSize; }

        auto getType() const { return type; }

        virtual void map() = 0;

        virtual void unmap() = 0;

        virtual void write(const void* data, size_t size = WHOLE_SIZE, size_t offset = 0) = 0;

    protected:
        Type    type;
        size_t  bufferSize{0};
        size_t  alignmentSize{0};
        void*   mappedAddress{nullptr};
    };

}