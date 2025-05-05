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

    std::shared_ptr<Vireo> Vireo::create(const Backend backend) {
        if (backend == Backend::VULKAN) {
            return std::make_shared<VKVireo>();
        }
#ifdef DIRECTX_BACKEND
        return std::make_shared<DXVireo>();
#endif
        throw Exception("Unsupported backend");
    }

    bool Vireo::isBackendSupported(const Backend backend) {
#ifdef DIRECTX_BACKEND
        if (backend == Backend::DIRECTX) { return true; }
#endif
        return backend == Backend::VULKAN;
    }

    std::shared_ptr<DescriptorLayout> Vireo::createDynamicUniformDescriptorLayout(
            const std::wstring& name) {
        const auto layout = _createDynamicUniformDescriptorLayout(name);
        layout->add(0, DescriptorType::UNIFORM_DYNAMIC);
        layout->build();
        return layout;
    }
}