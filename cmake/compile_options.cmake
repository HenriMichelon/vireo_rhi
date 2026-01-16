#
# Copyright (c) 2025-present Henri Michelon
#
# This software is released under the MIT License.
# https://opensource.org/licenses/MIT
#
function(compile_options TARGET_NAME )
    if(MSVC)
        target_compile_options(${TARGET_NAME} PRIVATE
                /nologo
                /W2
                /WX
                /std:c++latest
                /experimental:module
                /fp:fast
                /DUNICODE
                /D_UNICODE
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
                -Werror
                -pthread
        )
        target_link_libraries(${TARGET_NAME} -static)
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