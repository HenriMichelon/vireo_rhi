/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
#pragma once
#include "Libraries.h"
#include "vireo/Tools.h"

namespace vireo {

    inline string HrToString(const HRESULT hr) {
        char s_str[64] = {};
        sprintf_s(s_str, "HRESULT of 0x%08X", static_cast<UINT>(hr));
        return string(s_str);
    }

    inline void DieIfFailed(const HRESULT hr) {
        if (FAILED(hr)) {
            die(HrToString(hr));
        }
    }
}