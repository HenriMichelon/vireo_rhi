module;
#include "Libraries.h"
#include "Tools.h"

module dxvk.app;

import dxvk.app.win32;
import dxvk.backend.directx;
import dxvk.backend.vulkan;

namespace dxvk {

    Application::Application(UINT width, UINT height, std::wstring name) :
        m_width(width),
        m_height(height),
        m_title(name) {
        WCHAR assetsPath[512];
        GetAssetsPath(assetsPath, _countof(assetsPath));
        m_assetsPath = assetsPath;
        m_aspectRatio = static_cast<float>(width) / static_cast<float>(height);
        renderingBackEnd = std::make_shared<backend::VKRenderingBackEnd>(width, height);
        //renderingBackEnd = std::make_shared<backend::DXRenderingBackEnd>(width, height);
    }

    void Application::OnInit() {
        for (uint32_t i = 0; i < backend::SwapChain::FRAMES_IN_FLIGHT; i++) {
            framesData[i] = renderingBackEnd->createFrameData(i);
        }
    }

    void Application::OnUpdate() {
    }

    void Application::OnRender() {
        auto& swapChain = renderingBackEnd->getSwapChain();
        auto& frameData = *(framesData[swapChain->getCurrentFrameIndex()]);
        swapChain->prepare(frameData);
        //draw
        swapChain->present(frameData);
        swapChain->nextSwapChain();
    }

    void Application::OnDestroy() {
        // WaitForPreviousFrame();
    }

    std::wstring Application::GetAssetFullPath(LPCWSTR assetName)
    {
        return m_assetsPath + assetName;
    }

    // Helper function for setting the window's title text.
    void Application::SetCustomWindowText(LPCWSTR text)
    {
        std::wstring windowText = m_title + L": " + text;
        SetWindowText(Win32Application::getHwnd(), windowText.c_str());
    }

    // Generate a simple black and white checkerboard texture.
    std::vector<UINT8> Application::GenerateTextureData()
    {
        const UINT rowPitch = TextureWidth * TexturePixelSize;
        const UINT cellPitch = rowPitch >> 3;        // The width of a cell in the checkboard texture.
        const UINT cellHeight = TextureWidth >> 3;    // The height of a cell in the checkerboard texture.
        const UINT textureSize = rowPitch * TextureHeight;

        std::vector<UINT8> data(textureSize);
        UINT8* pData = &data[0];

        for (UINT n = 0; n < textureSize; n += TexturePixelSize)
        {
            UINT x = n % rowPitch;
            UINT y = n / rowPitch;
            UINT i = x / cellPitch;
            UINT j = y / cellHeight;

            if (i % 2 == j % 2)
            {
                pData[n] = 0x00;        // R
                pData[n + 1] = 0x00;    // G
                pData[n + 2] = 0x00;    // B
                pData[n + 3] = 0xff;    // A
            }
            else
            {
                pData[n] = 0xff;        // R
                pData[n + 1] = 0xff;    // G
                pData[n + 2] = 0xff;    // B
                pData[n + 3] = 0xff;    // A
            }
        }

        return data;
    }

}