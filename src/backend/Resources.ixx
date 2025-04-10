/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "Tools.h"
export module vireo.backend.resources;

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


    enum class Filter : uint8_t {
        NEAREST = 0,
        LINEAR  = 1,
    };

    enum class AddressMode : uint8_t {
        REPEAT          = 0,
        MIRRORED_REPEAT = 1,
        CLAMP_TO_EDGE   = 2,
        CLAMP_TO_BORDER = 3,
    };

    enum class MipMapMode : uint8_t {
        NEAREST = 0,
        LINEAR  = 1,
    };

    class Sampler {
    public:
        virtual ~Sampler() = default;
    };

    enum class ImageFormat : uint8_t {
        R8G8B8_SRGB     = 0,
        R8G8B8A8_SRGB   = 1,
    };

    class Image {
    public:
        static constexpr uint8_t pixelSize[] {
            3,
            4
        };

        Image(const ImageFormat format, const uint32_t width, const uint32_t height) :
            format{format},
            width{width},
            height{height} {}

        virtual ~Image() = default;

        auto getFormat() const { return format; }

        auto getWidth() const { return width; }

        auto getHeight() const { return height; }

        auto getSize() const { return width * height * pixelSize[static_cast<int>(format)]; }

    private:
        ImageFormat format;
        uint32_t width;
        uint32_t height;
    };

}