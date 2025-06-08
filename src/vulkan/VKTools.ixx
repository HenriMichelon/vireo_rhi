/*
 * Copyright (c) 2024-2025 Henri Michelon
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
*/
module;
#include "vireo/backend/vulkan/Libraries.h"
export module vireo.vulkan.tools;

import std;
import vireo.tools;

export namespace vireo {

    template <typename... Args>
    void vkCheck(const VkResult vr, Args&&... args) {
        if (vr != VK_SUCCESS) {
            throw Exception("Vulkan error #", vr, " : ", forward<Args>(args)...);
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