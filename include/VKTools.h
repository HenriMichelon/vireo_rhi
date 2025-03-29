#pragma once
#include "VKLibraries.h"
#include "Tools.h"
import std;

inline void ThrowIfFailed(VkResult  hr)
{
    if (hr != VK_SUCCESS)
    {
        throw std::runtime_error("");
    }
}
