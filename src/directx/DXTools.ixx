/*
 * Copyright (c) 2024-2025 Henri Michelon
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
*/
module;
#include "vireo/backend/directx/Libraries.h"
export module vireo.directx.tools;

import vireo.tools;

export namespace vireo {

    template <typename... Args>
    void dxCheck(const HRESULT hr, Args&&... args) {
        if (FAILED(hr)) {
             throw Exception("DirectX error : ", std::forward<Args>(args)...);
        }
    }

}