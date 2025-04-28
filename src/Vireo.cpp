/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module vireo;

import std;
import vireo.tools;
import vireo.vulkan;
#ifdef DIRECTX_BACKEND
import vireo.directx;
#endif

namespace vireo {

    std::unique_ptr<Vireo> Vireo::create(const Backend backend) {
        if (backend == Backend::VULKAN) {
            return std::make_unique<VKVireo>();
        }
#ifdef DIRECTX_BACKEND
        return std::make_unique<DXVireo>();
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