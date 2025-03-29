#pragma once
import std;
using namespace std;

inline void GetAssetsPath(_Out_writes_(pathSize) WCHAR* path, UINT pathSize)
{
    if (path == nullptr)
    {
        throw std::exception();
    }

    DWORD size = GetModuleFileName(nullptr, path, pathSize);
    if (size == 0 || size == pathSize)
    {
        // Method failed or path was truncated.
        throw std::exception();
    }

    WCHAR* lastSlash = wcsrchr(path, L'\\');
    if (lastSlash)
    {
        *(lastSlash + 1) = L'\0';
    }
}

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