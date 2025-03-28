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
        ${SRC_DIR}/app.cppm
)
add_executable(${PROJECT_NAME}
        ${D2D12Lite_DIR}/D3D12Lite.cpp
        ${D2D12Lite_DIR}/D3D12MemoryAllocator/D3D12MemAlloc.cpp
        ${D2D12Lite_DIR}/DXTex/BC.cpp
        ${D2D12Lite_DIR}/DXTex/BC4BC5.cpp
        ${D2D12Lite_DIR}/DXTex/BC6HBC7.cpp
        ${D2D12Lite_DIR}/DXTex/BCDirectCompute.cpp
        ${D2D12Lite_DIR}/DXTex/DirectXTexCompress.cpp
        ${D2D12Lite_DIR}/DXTex/DirectXTexCompressGPU.cpp
        ${D2D12Lite_DIR}/DXTex/DirectXTexConvert.cpp
        ${D2D12Lite_DIR}/DXTex/DirectXTexD3D11.cpp
        ${D2D12Lite_DIR}/DXTex/DirectXTexDDS.cpp
        ${D2D12Lite_DIR}/DXTex/DirectXTexFlipRotate.cpp
        ${D2D12Lite_DIR}/DXTex/DirectXTexImage.cpp
        ${D2D12Lite_DIR}/DXTex/DirectXTexMipmaps.cpp
        ${D2D12Lite_DIR}/DXTex/DirectXTexMisc.cpp
        ${D2D12Lite_DIR}/DXTex/DirectXTexNormalMaps.cpp
        ${D2D12Lite_DIR}/DXTex/DirectXTexPMAlpha.cpp
        ${D2D12Lite_DIR}/DXTex/DirectXTexResize.cpp
        ${D2D12Lite_DIR}/DXTex/DirectXTexTGA.cpp
        ${D2D12Lite_DIR}/DXTex/DirectXTexUtil.cpp
        ${D2D12Lite_DIR}/DXTex/DirectXTexWIC.cpp
        ${SRC_DIR}/main.cpp
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
target_link_libraries(${MY_TARGET} Xinput dinput8 dxguid dxgi d3d12)
target_link_libraries(${MY_TARGET} ${D2D12Lite_DIR}/dxc/lib/x64/dxcompiler.lib)
if(MINGW)
    target_link_options(${MY_TARGET} PRIVATE "-mwindows")
endif()
set_target_properties(${MY_TARGET} PROPERTIES
        WIN32_EXECUTABLE TRUE)

include(FetchContent)
include(cmake/std.cmake)
include(cmake/libraries.cmake)
