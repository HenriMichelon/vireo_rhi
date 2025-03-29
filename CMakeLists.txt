cmake_minimum_required(VERSION 3.29)

project(dx12)
set(MY_TARGET ${PROJECT_NAME})

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(MSVC)
    set(USE_STATIC_MSVC_RUNTIME_LIBRARY OFF)
endif()
include(cmake/compile_options.cmake)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(D2D12Lite_DIR ${CMAKE_CURRENT_SOURCE_DIR}/D3D12Lite)

#######################################################
set(PROJECT_FILES
        ${SRC_DIR}/BaseApplication.cppm
        ${SRC_DIR}/DX12Application.cppm
        ${SRC_DIR}/Win32Application.cppm
)
add_executable(${PROJECT_NAME}
        ${SRC_DIR}/main.cpp
        ${SRC_DIR}/BaseApplication.cpp
        ${SRC_DIR}/DX12Application.cpp
        ${SRC_DIR}/Win32Application.cpp
)
#######################################################

target_sources(${MY_TARGET}
        PUBLIC
        FILE_SET CXX_MODULES
        FILES
        ${PROJECT_FILES}
)

compile_options(${MY_TARGET})
target_include_directories(${MY_TARGET} PUBLIC ${INCLUDE_DIR})
target_include_directories(${MY_TARGET} PUBLIC ${D2D12Lite_DIR})
target_link_libraries(${MY_TARGET} Xinput dinput8 dxguid dxgi d3d12 d3dcompiler)
if(MINGW)
    target_link_options(${MY_TARGET} PRIVATE "-mwindows")
endif()
set_target_properties(${MY_TARGET} PROPERTIES
        WIN32_EXECUTABLE TRUE)

include(FetchContent)
include(cmake/std.cmake)
include(cmake/libraries.cmake)
