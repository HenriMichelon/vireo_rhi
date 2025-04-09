/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
#pragma once
#include "VKLibraries.h"
#include "Tools.h"

inline void DieIfFailed(const VkResult vr) {
    if (vr != VK_SUCCESS) {
        die(std::string(string_VkResult(vr)));
    }
}
