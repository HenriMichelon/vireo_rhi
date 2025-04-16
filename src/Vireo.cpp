/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/Tools.h"
module vireo;

#ifdef _WIN32
import vireo.directx;
#endif
import vireo.vulkan;

namespace vireo {

    unique_ptr<Vireo> Vireo::create(const Configuration& configuration) {
        if (configuration.backend == Backends::VULKAN) {
            return make_unique<VKVireo>(configuration);
        }
#ifdef _WIN32
        return make_unique<DXVireo>(configuration);
#endif
        die("Unsupported backend");
    }

}