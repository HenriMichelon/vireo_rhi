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
        // renderingBackEnd = std::make_shared<backend::VKRenderingBackEnd>(width, height);
        renderingBackEnd = std::make_shared<backend::DXRenderingBackEnd>(width, height);
    }


    void Application::OnInit() {
        for (int i = 0; i < backend::SwapChain::FRAMES_IN_FLIGHT; i++) {
            framesData[i] = renderingBackEnd->createFrameData();
        }
    }

    void Application::OnUpdate() {
    }

    void Application::OnRender() {
        auto& swapChain = renderingBackEnd->getSwapChain();
        swapChain->present(*(framesData[swapChain->getCurrentFrameIndex()]));
        WaitForPreviousFrame();
        swapChain->nextSwapChain();
    }

    void Application::OnDestroy() {
        WaitForPreviousFrame();
    }

    void Application::WaitForPreviousFrame() {
        // auto backend = std::static_pointer_cast<backend::DXRenderingBackEnd>(renderingBackEnd);
        // auto m_commandQueue = backend->getDXGraphicCommandQueue()->getCommandQueue();

        // WAITING FOR THE FRAME TO COMPLETE BEFORE CONTINUING IS NOT BEST PRACTICE.
        // This is code implemented as such for simplicity. The D3D12HelloFrameBuffering
        // sample illustrates how to use fences for efficient resource usage and to
        // maximize GPU utilization.

        // Signal and increment the fence value.
        // const UINT64 fence = m_fenceValue;
        // ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), fence));
        // m_fenceValue++;

        // Wait until the previous frame is finished.
        // if (m_fence->GetCompletedValue() < fence)
        // {
            // ThrowIfFailed(m_fence->SetEventOnCompletion(fence, m_fenceEvent));
            // WaitForSingleObject(m_fenceEvent, INFINITE);
        // }
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