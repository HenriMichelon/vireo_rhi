#
# Copyright (c) 2024-2025 Henri Michelon
#
# This software is released under the MIT License.
# https://opensource.org/licenses/MIT
#
###### Vulkan SDK
message(NOTICE "Searching for Vulkan SDK")
find_package(Vulkan REQUIRED)

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