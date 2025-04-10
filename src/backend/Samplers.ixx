/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "Tools.h"
export module vireo.backend.samplers;

export namespace vireo::backend {

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


}