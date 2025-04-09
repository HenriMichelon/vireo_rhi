/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
#pragma once
#include "VKLibraries.h"
#include "Tools.h"

inline void ThrowIfFailed(VkResult  hr)
{
    if (hr != VK_SUCCESS)
    {
        throw std::runtime_error("");
    }
}
