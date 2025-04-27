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

#ifdef DIRECTX_BACKEND
import vireo.directx;
#endif
import vireo.vulkan;

namespace vireo {

    unique_ptr<Vireo> Vireo::create(const Backend backend) {
        if (backend == Backend::VULKAN) {
            return make_unique<VKVireo>();
        }
#ifdef DIRECTX_BACKEND
        return make_unique<DXVireo>();
#endif
        throw Exception("Unsupported backend");
    }

    bool Vireo::isBackendSupported(const Backend backend) {
#ifdef DIRECTX_BACKEND
        if (backend == Backend::DIRECTX) { return true; }
#endif
        return backend == Backend::VULKAN;
    }
}