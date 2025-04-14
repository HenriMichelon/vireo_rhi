#
# Copyright (c) 2025-present Henri Michelon
#
# This software is released under the MIT License.
# https://opensource.org/licenses/MIT
#
cmake_minimum_required(VERSION 3.29)

#######################################################
project(vireo_ral)
set(VIREO_TARGET ${PROJECT_NAME})

#######################################################
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(MSVC)
    set(USE_STATIC_MSVC_RUNTIME_LIBRARY OFF)
endif()
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
add_compile_definitions(VK_NO_PROTOTYPES)
if(WIN32)
    add_compile_definitions(VK_USE_PLATFORM_WIN32_KHR WIN32_LEAN_AND_MEAN UNICODE _UNICODE)
endif ()

#######################################################
include(FetchContent)
include(cmake/shaders.cmake)
include(cmake/std.cmake)
include(cmake/compile_options.cmake)
include(cmake/libraries.cmake)

#######################################################
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SHADERS_SRC_DIR ${SRC_DIR}/shaders)
set(SHADERS_BUILD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shaders)

#######################################################
add_library(${VIREO_TARGET} STATIC
        ${SRC_DIR}/Vireo.cpp
        ${SRC_DIR}/directx/DXCommands.cpp
        ${SRC_DIR}/directx/DXResources.cpp
        ${SRC_DIR}/directx/DXDescriptors.cpp
        ${SRC_DIR}/directx/DXDevices.cpp
        ${SRC_DIR}/directx/DXPipelines.cpp
        ${SRC_DIR}/directx/DXRenderingBackEnd.cpp
        ${SRC_DIR}/directx/DXSwapChains.cpp
        ${SRC_DIR}/vulkan/VKCommands.cpp
        ${SRC_DIR}/vulkan/VKDevices.cpp
        ${SRC_DIR}/vulkan/VKDescriptors.cpp
        ${SRC_DIR}/vulkan/VKPipelines.cpp
        ${SRC_DIR}/vulkan/VKRenderingBackEnd.cpp
        ${SRC_DIR}/vulkan/VKResources.cpp
        ${SRC_DIR}/vulkan/VKSwapChains.cpp
        ${SRC_DIR}/vulkan/Vulkan.cpp
)

target_sources(${VIREO_TARGET}
    PUBLIC
    FILE_SET CXX_MODULES
    FILES
        ${SRC_DIR}/Configuration.ixx
        ${SRC_DIR}/Vireo.ixx
        ${SRC_DIR}/directx/DXCommands.ixx
        ${SRC_DIR}/directx/DXResources.ixx
        ${SRC_DIR}/directx/DXDescriptors.ixx
        ${SRC_DIR}/directx/DXDevices.ixx
        ${SRC_DIR}/directx/DXFrameData.ixx
        ${SRC_DIR}/directx/DXPipelines.ixx
        ${SRC_DIR}/directx/DXRenderingBackEnd.ixx
        ${SRC_DIR}/directx/DXSwapChains.ixx
        ${SRC_DIR}/vulkan/VKCommands.ixx
        ${SRC_DIR}/vulkan/VKDevices.ixx
        ${SRC_DIR}/vulkan/VKDescriptors.ixx
        ${SRC_DIR}/vulkan/VKFrameData.ixx
        ${SRC_DIR}/vulkan/VKPipelines.ixx
        ${SRC_DIR}/vulkan/VKRenderingBackEnd.ixx
        ${SRC_DIR}/vulkan/VKResources.ixx
        ${SRC_DIR}/vulkan/VKSwapChains.ixx
)

compile_options(${VIREO_TARGET})
target_include_directories(${VIREO_TARGET} PRIVATE ${INCLUDE_DIR} ${Vulkan_INCLUDE_DIRS} ${DirectXH_DIR}/include/directx)
target_link_libraries(${VIREO_TARGET} std-cxx-modules)
target_precompile_headers(${VIREO_TARGET} PRIVATE
        "<vulkan/vulkan.h>"
        "<vulkan/vk_enum_string_helper.h>"
)
if(WIN32)
    target_precompile_headers(${VIREO_TARGET} PRIVATE
            "<windows.h>"
            "<cstdint>"
            "<d3d12.h>"
            "<d3dx12.h>"
            "<dxgi1_6.h>"
            "<D3Dcompiler.h>"
            "<DirectXMath.h>"
            "<wrl.h>"
    )
    target_link_libraries(${VIREO_TARGET} DirectX-Headers dxgi d3d12)
    set_target_properties(${VIREO_TARGET} PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

#######################################################
# Slang shaders
file(MAKE_DIRECTORY ${SHADERS_BUILD_DIR})
file(GLOB_RECURSE SHADERS_SOURCE_FILES
    "${SHADERS_SRC_DIR}/*.slang"
)
add_shaders(${VIREO_TARGET}_shaders ${SHADERS_BUILD_DIR} ${SHADERS_SRC_DIR} ${SHADERS_SOURCE_FILES})
add_dependencies(${VIREO_TARGET} ${VIREO_TARGET}_shaders)

