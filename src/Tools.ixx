/*
 * Copyright (c) 2024-2025 Henri Michelon
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
*/
module;
#ifdef _WIN32
#include <windows.h>
#endif
export module vireo.tools;

import std;

export namespace vireo {

    class Exception : public std::exception {
    public:
        template <typename... Args>
        Exception(Args... args) {
            std::ostringstream oss;
            (oss << ... << args);
            message = oss.str();
#ifdef _DEBUG
#ifdef _WIN32
            if (IsDebuggerPresent()) {
                OutputDebugStringA(message.c_str());
#elifdef __linux__
            std::cerr << message << std::endl;
#endif
#ifdef __has_builtin
                __builtin_debugtrap();
#endif
#ifdef _MSC_VER
                __debugbreak();
#endif
#ifdef _WIN32
            }
#endif
#endif
        }

        const char* what() const noexcept override {
            return message.c_str();
        }

    private:
        std::string message;
    };

}

export namespace std {

#ifdef _WIN32
    inline std::string to_string(const std::wstring& wstr) {
        if (wstr.empty())
            return {};
        const int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), static_cast<int>(wstr.size()), nullptr, 0, nullptr, nullptr);
        std::string result(size_needed, 0);
        WideCharToMultiByte(CP_UTF8, 0, wstr.data(), static_cast<int>(wstr.size()), result.data(), size_needed, nullptr, nullptr);
        return result;
    }

    inline std::wstring to_wstring(const std::string& str) {
        if (str.empty())
            return {};
        const int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), nullptr, 0);
        std::wstring result(size_needed, 0);
        MultiByteToWideChar(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), size_needed);
        return result;
    }
#else

    inline string to_string(const std::wstring &wstr) {
        wstring_convert<codecvt_utf8_utf16<wchar_t>> conv;
        return conv.to_bytes(wstr);
    }

    inline wstring to_wstring(const std::string &str) {
        wstring_convert<codecvt_utf8_utf16<wchar_t>> conv;
        return conv.from_bytes(str);
    }

#endif


}