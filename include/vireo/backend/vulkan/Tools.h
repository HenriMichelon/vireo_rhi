/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
#pragma once
#include "vireo/backend/vulkan/Libraries.h"
#include "vireo/Tools.h"

namespace vireo {

    inline void DieIfFailed(const VkResult vr) {
        if (vr != VK_SUCCESS) {
            die(std::string(string_VkResult(vr)));
        }
    }

#ifdef _DEBUG
    inline void vkSetObjectName(const VkDevice device, const uint64_t objectHandle, const VkObjectType objectType, const std::string& name) {
        const auto func = reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(vkGetDeviceProcAddr(device, "vkSetDebugUtilsObjectNameEXT"));
        if (func) {
            const auto nameInfo = VkDebugUtilsObjectNameInfoEXT{
                .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
                .objectType = objectType,
                .objectHandle = objectHandle,
                .pObjectName = name.c_str(),
            };
            func(device, &nameInfo);
        }
    }
#endif

}