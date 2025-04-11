#
# Copyright (c) 2025-present Henri Michelon
#
# This software is released under the MIT License.
# https://opensource.org/licenses/MIT
#
cmake_minimum_required(VERSION 3.29)

#######################################################
project(vireo_ral)
set(MY_TARGET ${PROJECT_NAME})

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

#######################################################
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(MSVC)
    set(USE_STATIC_MSVC_RUNTIME_LIBRARY OFF)
endif()
include(cmake/compile_options.cmake)

#######################################################
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SHADERS_SRC_DIR ${SRC_DIR}/shaders)
set(SHADERS_BUILD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shaders)

#######################################################
add_executable(${MY_TARGET}
        ${SRC_DIR}/Application.cpp
        ${SRC_DIR}/os/windows/Win32Application.cpp
        ${SRC_DIR}/os/windows/WinMain.cpp
        ${SRC_DIR}/backend/directx/DXResources.cpp
        ${SRC_DIR}/backend/directx/DXDescriptors.cpp
        ${SRC_DIR}/backend/directx/DXDevice.cpp
        ${SRC_DIR}/backend/directx/DXRenderingBackEnd.cpp
        ${SRC_DIR}/backend/vulkan/VKResources.cpp
        ${SRC_DIR}/backend/vulkan/VKDevice.cpp
        ${SRC_DIR}/backend/vulkan/VKDescriptors.cpp
        ${SRC_DIR}/backend/vulkan/VKRenderingBackEnd.cpp
        ${SRC_DIR}/backend/vulkan/Vulkan.cpp
)

target_sources(${MY_TARGET}
    PUBLIC
    FILE_SET CXX_MODULES
    FILES
        ${SRC_DIR}/Application.ixx
        ${SRC_DIR}/os/windows/Win32Application.ixx
        ${SRC_DIR}/backend/Descriptors.ixx
        ${SRC_DIR}/backend/Device.ixx
        ${SRC_DIR}/backend/FrameData.ixx
        ${SRC_DIR}/backend/RenderingBackEnd.ixx
        ${SRC_DIR}/backend/Resources.ixx
        ${SRC_DIR}/backend/directx/DXResources.ixx
        ${SRC_DIR}/backend/directx/DXDescriptors.ixx
        ${SRC_DIR}/backend/directx/DXDevice.ixx
        ${SRC_DIR}/backend/directx/DXFrameData.ixx
        ${SRC_DIR}/backend/directx/DXRenderingBackEnd.ixx
        ${SRC_DIR}/backend/vulkan/VKResources.ixx
        ${SRC_DIR}/backend/vulkan/VKDevice.ixx
        ${SRC_DIR}/backend/vulkan/VKDescriptors.ixx
        ${SRC_DIR}/backend/vulkan/VKFrameData.ixx
        ${SRC_DIR}/backend/vulkan/VKRenderingBackEnd.ixx
)
compile_options(${MY_TARGET})

include(FetchContent)
include(cmake/libraries.cmake)
include(cmake/shaders.cmake)
include(cmake/std.cmake)

#######################################################
# HLSL shaders
file(MAKE_DIRECTORY ${SHADERS_BUILD_DIR})
file(GLOB_RECURSE HLSL_SOURCE_FILES
    "${SHADERS_SRC_DIR}/*.hlsl"
)
add_shaders(${MY_TARGET}_shaders ${SHADERS_BUILD_DIR} ${SHADERS_SRC_DIR} ${HLSL_SOURCE_FILES})
add_dependencies(${MY_TARGET} ${MY_TARGET}_shaders)

#######################################################
target_include_directories(${MY_TARGET} PUBLIC ${INCLUDE_DIR})
target_include_directories(${MY_TARGET} PUBLIC ${D2D12Lite_DIR})
target_link_libraries(${MY_TARGET} Xinput dinput8 dxguid dxgi d3d12 d3dcompiler)
if(MINGW)
    target_link_options(${MY_TARGET} PRIVATE "-mwindows")
endif()
set_target_properties(${MY_TARGET} PROPERTIES
        WIN32_EXECUTABLE TRUE)

#######################################################
