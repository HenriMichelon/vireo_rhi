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

    /**
     * Exception type thrown by the RHI for unrecoverable errors.
     * In debug builds with a debugger attached it also triggers a breakpoint.
     */
    class Exception : public std::exception {
    public:
        /**
         * Constructs an exception by concatenating all arguments into a single message string.
         * @tparam Args Variadic argument types — any type supported by `operator<<` on `std::ostringstream`.
         */
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

        /** Returns the formatted error message. */
        const char* what() const noexcept override {
            return message.c_str();
        }

    private:
        std::string message;
    };

}

export namespace std {

#ifdef _WIN32
    /** Converts a UTF-16 wide string to a UTF-8 `std::string`. */
    inline std::string to_string(const std::wstring& wstr) {
        if (wstr.empty())
            return {};
        const int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), static_cast<int>(wstr.size()), nullptr, 0, nullptr, nullptr);
        std::string result(size_needed, 0);
        WideCharToMultiByte(CP_UTF8, 0, wstr.data(), static_cast<int>(wstr.size()), result.data(), size_needed, nullptr, nullptr);
        return result;
    }

    /** Converts a UTF-8 `std::string` to a UTF-16 wide string. */
    inline std::wstring to_wstring(const std::string& str) {
        if (str.empty())
            return {};
        const int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), nullptr, 0);
        std::wstring result(size_needed, 0);
        MultiByteToWideChar(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), size_needed);
        return result;
    }
#else

    /** Converts a UTF-16 wide string to a UTF-8 `std::string`. */
    inline string to_string(const std::wstring &wstr) {
        wstring_convert<codecvt_utf8_utf16<wchar_t>> conv;
        return conv.to_bytes(wstr);
    }

    /** Converts a UTF-8 `std::string` to a UTF-16 wide string. */
    inline wstring to_wstring(const std::string &str) {
        wstring_convert<codecvt_utf8_utf16<wchar_t>> conv;
        return conv.from_bytes(str);
    }

#endif


}