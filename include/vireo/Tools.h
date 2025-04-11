/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
#pragma once
#include "vireo/Libraries.h"

namespace vireo {

    void die(std::convertible_to<std::string_view> auto&& ...s) {
        std::stringstream stringstream;
        for (const auto v : std::initializer_list<std::string_view>{ s... }) {
            stringstream << v << " ";
        }
        std::cerr << stringstream.str() << std::endl;
#if defined(_DEBUG)
#if defined(__has_builtin)
        __builtin_debugtrap();
#elif defined(_MSC_VER)
        __debugbreak();
#else
        throw runtime_error(stringstream.str());
#endif
#endif
    }

    inline std::string wstring_to_string(const std::wstring &wstr) {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
        return conv.to_bytes(wstr);
    }

    inline std::wstring string_to_wstring(const std::string &str) {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
        return conv.from_bytes(str);
    }

}