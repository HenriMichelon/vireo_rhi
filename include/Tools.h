#pragma once
import std;
using namespace std;

void die(convertible_to<string_view> auto&& ...s) {
    stringstream stringstream;
    for (const auto v : initializer_list<string_view>{ s... }) {
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

inline string wstring_to_string(const std::wstring &wstr) {
    wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
    return conv.to_bytes(wstr);
}

inline wstring string_to_wstring(const std::string &str) {
    wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
    return conv.from_bytes(str);
}