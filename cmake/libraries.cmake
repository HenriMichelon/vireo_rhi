#
# Copyright (c) 2024-2025 Henri Michelon
#
# This software is released under the MIT License.
# https://opensource.org/licenses/MIT
#
###### Vulkan SDK
message(NOTICE "Searching for Vulkan SDK")
find_package(Vulkan REQUIRED)
target_include_directories(${VIREO_TARGET} PUBLIC ${Vulkan_INCLUDE_DIRS})

###### DirectX Headers
message(NOTICE "Fetching DirectX-Headers from https://github.com/microsoft/DirectX-Headers.git...")
FetchContent_Declare(
        DirectX-Headers
        GIT_REPOSITORY https://github.com/microsoft/DirectX-Headers.git
        GIT_TAG main
)
FetchContent_MakeAvailable(DirectX-Headers)
set(DirectXH_DIR ${CMAKE_BINARY_DIR}/_deps/directx-headers-src)
target_include_directories(${VIREO_TARGET} PUBLIC ${DirectXH_DIR}/include/directx)
target_link_libraries(${VIREO_TARGET} DirectX-Headers)
