/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
#pragma once
#include "vireo/Libraries.h"

namespace vireo {

    void die(convertible_to<string_view> auto&& ...s) {
        stringstream stringstream;
        for (const auto v : initializer_list<string_view>{ s... }) {
            stringstream << v << " ";
        }
        cerr << stringstream.str() << endl;
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

    inline string wstring_to_string(const wstring &wstr) {
        wstring_convert<codecvt_utf8_utf16<wchar_t>> conv;
        return conv.to_bytes(wstr);
    }

    inline wstring string_to_wstring(const string &str) {
        wstring_convert<codecvt_utf8_utf16<wchar_t>> conv;
        return conv.from_bytes(str);
    }

}