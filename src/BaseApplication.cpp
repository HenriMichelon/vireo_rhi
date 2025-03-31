module;
#include "Libraries.h"
#include "Tools.h"

module dxvk.app;

import dxvk.app.win32;

namespace dxvk {

    BaseApplication::BaseApplication(UINT width, UINT height, std::wstring name) :
        m_width(width),
        m_height(height),
        m_title(name) {
        WCHAR assetsPath[512];
        GetAssetsPath(assetsPath, _countof(assetsPath));
        m_assetsPath = assetsPath;

        m_aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    }

    std::wstring BaseApplication::GetAssetFullPath(LPCWSTR assetName)
    {
        return m_assetsPath + assetName;
    }

    // Helper function for setting the window's title text.
    void BaseApplication::SetCustomWindowText(LPCWSTR text)
    {
        std::wstring windowText = m_title + L": " + text;
        SetWindowText(Win32Application::GetHwnd(), windowText.c_str());
    }

}