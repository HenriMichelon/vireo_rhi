#
# Copyright (c) 2024-2025 Henri Michelon
#
# This software is released under the MIT License.
# https://opensource.org/licenses/MIT
#
###### Vulkan SDK
message(NOTICE "Searching for Vulkan SDK")
find_package(Vulkan REQUIRED)
message("Vulkan " ${Vulkan_VERSION} " found")

###### DirectX Headers
if (DIRECTX_BACKEND)
    message(NOTICE "Fetching DirectX-Headers from https://github.com/microsoft/DirectX-Headers.git...")
    FetchContent_Declare(
            DirectX-Headers
            GIT_REPOSITORY https://github.com/microsoft/DirectX-Headers.git
            GIT_TAG main
    )
    FetchContent_MakeAvailable(DirectX-Headers)
    set(DirectXH_DIR ${CMAKE_BINARY_DIR}/_deps/directx-headers-src)
endif ()

if (LUA_BINDING)
message(NOTICE "Fetching Lua-cmake...")
FetchContent_Declare(
        lua_cmake
        GIT_REPOSITORY https://github.com/lubgr/lua-cmake.git
        GIT_TAG        master
)
FetchContent_MakeAvailable(lua_cmake)
endif()