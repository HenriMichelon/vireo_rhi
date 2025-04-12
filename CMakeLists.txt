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

#######################################################
include(FetchContent)
include(cmake/shaders.cmake)
include(cmake/std.cmake)
include(cmake/compile_options.cmake)

#######################################################
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SHADERS_SRC_DIR ${SRC_DIR}/shaders)
set(SHADERS_BUILD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shaders)

#######################################################
add_library(${VIREO_TARGET} STATIC
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

target_sources(${VIREO_TARGET}
    PUBLIC
    FILE_SET CXX_MODULES
    FILES
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

compile_options(${VIREO_TARGET})
target_include_directories(${VIREO_TARGET} PUBLIC ${INCLUDE_DIR})
if(WIN32)
target_link_libraries(${VIREO_TARGET} std-cxx-modules Xinput dinput8 dxguid dxgi d3d12 d3dcompiler)
if(MINGW)
    target_link_options(${VIREO_TARGET} PRIVATE "-mwindows")
endif()
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

include(cmake/libraries.cmake)
