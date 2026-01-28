#
# Copyright (c) 2025-present Henri Michelon
#
# This software is released under the MIT License.
# https://opensource.org/licenses/MIT
#
function(vireo_compile_options TARGET_NAME)
    set(CMAKE_CXX_STANDARD 23 CACHE INTERNAL "")
    set(CMAKE_CXX_STANDARD_REQUIRED ON CACHE INTERNAL "")
    set(CMAKE_CXX_EXTENSIONS OFF CACHE INTERNAL "")
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Release CACHE INTERNAL "")
    endif()
    if(WIN32)
        add_compile_definitions(WIN32_LEAN_AND_MEAN UNICODE _UNICODE NOMINMAX)
        if (DIRECTX_BACKEND)
            add_compile_definitions(DIRECTX_BACKEND)
            target_link_libraries(${TARGET_NAME} Xinput dinput8 dxguid dxgi d3d12 d3dcompiler uuid )
        endif ()
        if(MINGW)
            target_link_options(${TARGET_NAME} PRIVATE "-mwindows")
        endif()
        set_target_properties(${TARGET_NAME} PROPERTIES  WIN32_EXECUTABLE TRUE)
    elseif (LINUX AND NOT APPLE)
        add_compile_definitions(USE_SDL3)
    endif ()
    if(MSVC)
        set(USE_STATIC_MSVC_RUNTIME_LIBRARY ON CACHE INTERNAL "")
        target_compile_options(${TARGET_NAME} PRIVATE )
        target_compile_options(${TARGET_NAME} PRIVATE
                /nologo
                /W2
                /WX
                /std:c++latest
                /experimental:module
                /fp:fast
                /wd4291  # for Flecs 'new' placement operator
        )
        if (CMAKE_BUILD_TYPE STREQUAL "Debug")
            target_compile_options(${TARGET_NAME} PRIVATE
                    /Od
                    /GS
                    /Ob0
                    /RTC1
                    /Wv:18
            )
            add_definitions(-D_DEBUG)
            if (USE_STATIC_MSVC_RUNTIME_LIBRARY)
                set_property(TARGET ${TARGET_NAME} PROPERTY
                        MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
            else()
                set_property(TARGET ${TARGET_NAME} PROPERTY
                        MSVC_RUNTIME_LIBRARY "MultiThreadedDebugDLL")
            endif()
        else()
            target_compile_options(${TARGET_NAME} PRIVATE
                    /O2
                    /GS-
                    /Gy
                    /Oi
                    /Ot
                    /Wv:18
            )
            if (USE_STATIC_MSVC_RUNTIME_LIBRARY)
                set_property(TARGET ${TARGET_NAME} PROPERTY
                        MSVC_RUNTIME_LIBRARY "MultiThreaded")
            else()
                set_property(TARGET ${TARGET_NAME} PROPERTY
                        MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
            endif()
        endif()
    else()
        target_compile_options(${TARGET_NAME} PRIVATE
                -Wno-deprecated-declarations
                -Wno-nullability-completeness
                -Wno-unused-command-line-argument
                -Werror
                -pthread
        )
        if(WIN32)
            target_link_libraries(${TARGET_NAME} -static)
        else()
            target_compile_options(${TARGET_NAME} PRIVATE -stdlib=libc++)
            target_link_options(${TARGET_NAME} PRIVATE -stdlib=libc++)
        endif ()
        if (CMAKE_BUILD_TYPE STREQUAL "Debug")
            add_compile_definitions(_DEBUG)
            target_compile_options(${TARGET_NAME} PRIVATE
                -O0
                -D_DEBUG
            )
        else()
            target_compile_options(${TARGET_NAME} PRIVATE
                -O3
                -DNDEBUG
            )
        endif()
    endif()
endfunction()