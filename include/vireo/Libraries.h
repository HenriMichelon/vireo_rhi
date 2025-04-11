/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
#pragma once

#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #ifndef UNICODE
        #define UNICODE
    #endif
    #ifndef _UNICODE
        #define _UNICODE
    #endif
    #include <windows.h>
#endif

import glm;
import std;

#ifdef __clang__
    #include <cstdint>
#endif
