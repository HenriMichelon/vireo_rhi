/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "lua.h"
export module vireo.lua;

import std;
export import lua_bridge;

export namespace vireo {

    class LuaBindings {
    public:
        static void _register(lua_State*L);
    };

}