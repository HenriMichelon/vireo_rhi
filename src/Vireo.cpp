/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/Libraries.h"
module vireo;

import vireo.tools;

#ifdef _WIN32
import vireo.directx;
#endif
import vireo.vulkan;

namespace vireo {

    unique_ptr<Vireo> Vireo::create(const Backend backend) {
        if (backend == Backend::VULKAN) {
            return make_unique<VKVireo>();
        }
#ifdef _WIN32
        return make_unique<DXVireo>();
#endif
        throw Exception("Unsupported backend");
    }

}