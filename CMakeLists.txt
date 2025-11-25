#
# Copyright (c) 2025-present Henri Michelon
#
# This software is released under the MIT License.
# https://opensource.org/licenses/MIT
#
cmake_minimum_required(VERSION 3.29)

#######################################################
project(vireo_rhi)
set(VIREO_TARGET ${PROJECT_NAME})
if(NOT DEFINED DIRECTX_BACKEND)
    set(DIRECTX_BACKEND ON)
endif()
set(LUA_BINDINGS ON)

#######################################################
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
add_compile_definitions(VK_NO_PROTOTYPES)
if(WIN32)
    add_compile_definitions(VK_USE_PLATFORM_WIN32_KHR UNICODE _UNICODE)
    if(MSVC)
        set(USE_STATIC_MSVC_RUNTIME_LIBRARY OFF)
    endif()
    if (DIRECTX_BACKEND)
        add_compile_definitions(DIRECTX_BACKEND)
    endif ()
endif ()

#######################################################
include(FetchContent)
include(cmake/std.cmake)
include(cmake/compile_options.cmake)
include(cmake/libraries.cmake)

#######################################################
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

#######################################################
if (${DIRECTX_BACKEND})
set(DIRECTX_SOURCES
        ${SRC_DIR}/directx/DXCommands.cpp
        ${SRC_DIR}/directx/DXDescriptors.cpp
        ${SRC_DIR}/directx/DXDevices.cpp
        ${SRC_DIR}/directx/DXPipelines.cpp
        ${SRC_DIR}/directx/DXResources.cpp
        ${SRC_DIR}/directx/DXSwapChains.cpp
        ${SRC_DIR}/directx/DXVireo.cpp
)
endif ()
if (${LUA_BINDINGS})
    set(LUA_BINDINGS_SOURCES
        ${SRC_DIR}/lua/LuaBindings.cpp
    )
    set(LUA_BINDINGS_MODULES
        ${SRC_DIR}/lua/LuaBindings.ixx
        ${SRC_DIR}/lua/LuaBridge.ixx
    )
endif ()
add_library(${VIREO_TARGET} STATIC
        ${SRC_DIR}/Vireo.cpp
        ${DIRECTX_SOURCES}
        ${SRC_DIR}/vulkan/VKCommands.cpp
        ${SRC_DIR}/vulkan/VKDevices.cpp
        ${SRC_DIR}/vulkan/VKDescriptors.cpp
        ${SRC_DIR}/vulkan/VKPipelines.cpp
        ${SRC_DIR}/vulkan/VKResources.cpp
        ${SRC_DIR}/vulkan/VKSwapChains.cpp
        ${SRC_DIR}/vulkan/VKVireo.cpp
        ${SRC_DIR}/vulkan/Vulkan.cpp
        ${LUA_BINDINGS_SOURCES}
)

if (${DIRECTX_BACKEND})
set(DIRECTX_MODULES
        ${SRC_DIR}/directx/DXCommands.ixx
        ${SRC_DIR}/directx/DXDescriptors.ixx
        ${SRC_DIR}/directx/DXDevices.ixx
        ${SRC_DIR}/directx/DXPipelines.ixx
        ${SRC_DIR}/directx/DXResources.ixx
        ${SRC_DIR}/directx/DXSwapChains.ixx
        ${SRC_DIR}/directx/DXTools.ixx
        ${SRC_DIR}/directx/DXVireo.ixx
)
endif ()

target_sources(${VIREO_TARGET}
        PUBLIC
        FILE_SET CXX_MODULES
        FILES
        ${SRC_DIR}/Tools.ixx
        ${SRC_DIR}/Vireo.ixx
        ${DIRECTX_MODULES}
        ${SRC_DIR}/vulkan/VKCommands.ixx
        ${SRC_DIR}/vulkan/VKDevices.ixx
        ${SRC_DIR}/vulkan/VKDescriptors.ixx
        ${SRC_DIR}/vulkan/VKPipelines.ixx
        ${SRC_DIR}/vulkan/VKResources.ixx
        ${SRC_DIR}/vulkan/VKSwapChains.ixx
        ${SRC_DIR}/vulkan/VKTools.ixx
        ${SRC_DIR}/vulkan/VKVireo.ixx
        ${LUA_BINDINGS_MODULES}
)

compile_options(${VIREO_TARGET})
target_include_directories(${VIREO_TARGET} PRIVATE ${INCLUDE_DIR} ${Vulkan_INCLUDE_DIRS})
if(DIRECTX_BACKEND)
    target_include_directories(${VIREO_TARGET} PRIVATE ${DirectXH_DIR}/include/directx)
endif ()
target_link_libraries(${VIREO_TARGET} std-cxx-modules)
if (LUA_BINDINGS)
    target_link_libraries(${VIREO_TARGET} lua)
    target_include_directories(${VIREO_TARGET} PRIVATE ${lua_cmake_SOURCE_DIR}/upstream)
endif ()
target_precompile_headers(${VIREO_TARGET} PRIVATE
        "<cstdint>"
)
if(WIN32)
    target_compile_definitions(${VIREO_TARGET} PRIVATE WIN32_LEAN_AND_MEAN)
    set_target_properties(${VIREO_TARGET} PROPERTIES WIN32_EXECUTABLE TRUE)
    if (DIRECTX_BACKEND)
        target_link_libraries(${VIREO_TARGET} DirectX-Headers dxgi d3d12)
    endif()
endif ()
set_property(TARGET ${VIREO_TARGET} PROPERTY COMPILE_WARNING_AS_ERROR ON)

#######################################################
find_program(DOXYPRESS_EXECUTABLE doxypress)

if (DOXYPRESS_EXECUTABLE)
    message(STATUS "DoxyPress found: ${DOXYPRESS_EXECUTABLE}")
    set(DOCS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/docs")

    add_custom_target(vireo_docs
            COMMAND ${DOXYPRESS_EXECUTABLE}
            COMMAND ${CMAKE_COMMAND} -E copy
                ${DOCS_DIRECTORY}/navtree.js
                ${DOCS_DIRECTORY}/html/navtree.js
            WORKING_DIRECTORY ${DOCS_DIRECTORY}
            COMMENT "Generating documentation with DoxyPress"
            VERBATIM
    )
else()
    message(STATUS "DoxyPress not found, doxypress target will not be available.")
endif()