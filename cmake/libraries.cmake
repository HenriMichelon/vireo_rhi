#
# Copyright (c) 2024-2025 Henri Michelon
#
# This software is released under the MIT License.
# https://opensource.org/licenses/MIT
#
######################################################
find_package(Vulkan REQUIRED)
message("Vulkan " ${Vulkan_VERSION} " found")

######################################################
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

######################################################
if (LUA_BINDING)
    message(NOTICE "Fetching Lua-cmake...")
    FetchContent_Declare(
            lua_cmake
            GIT_REPOSITORY https://github.com/lubgr/lua-cmake.git
            GIT_TAG        master
    )
    FetchContent_MakeAvailable(lua_cmake)
endif()

######################################################
if (USE_SDL3)
    message(NOTICE "Fetching SDL3...")
    FetchContent_Declare(
            SDL3
            GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
            GIT_TAG        release-3.4.0
    )
    set(SDL_SHARED OFF CACHE BOOL "" FORCE)
    set(SDL_STATIC ON  CACHE BOOL "" FORCE)

    set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(SDL_INSTALL OFF CACHE BOOL "" FORCE)
    set(SDL_TEST OFF CACHE BOOL "" FORCE)

    set(SDL_AUDIO  ON  CACHE BOOL "" FORCE)
    set(SDL_EVENTS ON  CACHE BOOL "" FORCE)
    set(SDL_VIDEO  ON  CACHE BOOL "" FORCE)
    set(SDL_GAMEPAD ON CACHE BOOL "" FORCE)
    set(SDL_HIDAPI ON CACHE BOOL "" FORCE)
    set(SDL_HIDAPI_JOYSTICK ON CACHE BOOL "" FORCE)
    set(SDL_JOYSTICK ON CACHE BOOL "" FORCE)
    set(SDL_VULKAN ON CACHE BOOL "" FORCE)
    set(SDL_WAYLAND ON CACHE BOOL "" FORCE)
    set(SDL_X11 ON CACHE BOOL "" FORCE)

    set(SDL_CAMERA OFF CACHE BOOL "" FORCE)
    set(SDL_CLOCK_GETTIME OFF CACHE BOOL "" FORCE)
    set(SDL_DBUS OFF CACHE BOOL "" FORCE)
    set(SDL_DISKAUDIO OFF CACHE BOOL "" FORCE)
    set(SDL_DISKAUDIO OFF CACHE BOOL "" FORCE)
    set(SDL_DUMMYAUDIO OFF CACHE BOOL "" FORCE)
    set(SDL_DUMMYVIDEO OFF CACHE BOOL "" FORCE)
    set(SDL_FRIBIDI_SHARED OFF CACHE BOOL "" FORCE)
    set(SDL_GPU OFF CACHE BOOL "" FORCE)
    set(SDL_HAPTIC OFF CACHE BOOL "" FORCE)
    set(SDL_IBUS OFF CACHE BOOL "" FORCE)
    set(SDL_KMSDRM OFF CACHE BOOL "" FORCE)
    set(SDL_KMSDRM_SHARED OFF CACHE BOOL "" FORCE)
    set(SDL_LASX OFF CACHE BOOL "" FORCE)
    set(SDL_LIBTHAI OFF CACHE BOOL "" FORCE)
    set(SDL_LIBUDEV OFF CACHE BOOL "" FORCE)
    set(SDL_LIBURING OFF CACHE BOOL "" FORCE)
    set(SDL_OFFSCREEN OFF CACHE BOOL "" FORCE)
    set(SDL_OPENGL OFF CACHE BOOL "" FORCE)
    set(SDL_OPENGLES OFF CACHE BOOL "" FORCE)
    set(SDL_OPENVR OFF CACHE BOOL "" FORCE)
    set(SDL_POWER OFF CACHE BOOL "" FORCE)
    set(SDL_RENDER OFF CACHE BOOL "" FORCE)
    set(SDL_SENSOR OFF CACHE BOOL "" FORCE)

    FetchContent_MakeAvailable(SDL3)
endif ()
