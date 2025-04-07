module;
#include "Libraries.h"
#include "Tools.h"

module dxvk.app;

import dxvk.app.win32;
import dxvk.backend.directx;
import dxvk.backend.vulkan;

namespace dxvk {

    Application::Application(UINT width, UINT height, std::wstring name) :
        width(width),
        height(height),
        title(name) {
        aspectRatio = static_cast<float>(width) / static_cast<float>(height);
        // renderingBackEnd = std::make_shared<backend::VKRenderingBackEnd>(width, height);
        renderingBackEnd = std::make_shared<backend::DXRenderingBackEnd>(width, height);
    }

    void Application::onInit() {
        for (uint32_t i = 0; i < backend::SwapChain::FRAMES_IN_FLIGHT; i++) {
            framesData[i] = renderingBackEnd->createFrameData(i);
            graphicCommandAllocator[i] = renderingBackEnd->getDevice()->createCommandAllocator(backend::CommandAllocator::GRAPHIC);
            graphicCommandList[i] = graphicCommandAllocator[i]->createCommandList();
        }

        auto attributes = std::vector{
            backend::VertexInputLayout::AttributeDescription{"POSITION", backend::VertexInputLayout::R32G32B32_FLOAT, 0},
            backend::VertexInputLayout::AttributeDescription{"COLOR",    backend::VertexInputLayout::R32G32B32_FLOAT, 12}
        };
        auto defaultVertexInputLayout = renderingBackEnd->createVertexLayout(sizeof(Vertex), attributes);
        //  dxc -T "vs_5_0" -E "VSMain" -Fo shaders1_vert.cso .\shaders1.hlsl
        auto vertexShader = renderingBackEnd->createShaderModule(
            "shaders/shaders1.hlsl",
            "VSMain");
        // dxc -T "ps_5_0" -E "PSMain" -Fo shaders1_frag.cso .\shaders1.hlsl
        auto fragmentShader = renderingBackEnd->createShaderModule(
            "shaders/shaders1.hlsl",
            "PSMain");
        pipelineResources["default"] = renderingBackEnd->createPipelineResources();
        pipelines["default"] = renderingBackEnd->createPipeline(
            *pipelineResources["default"],
            *defaultVertexInputLayout,
            *vertexShader,
            *fragmentShader);

        auto triangleVertices = std::vector<Vertex> {
            { { 0.0f, 0.25f * aspectRatio, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
            { { 0.25f, -0.25f * aspectRatio, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
            { { -0.25f, -0.25f * aspectRatio, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
        };
        const UINT vertexBufferSize = sizeof(triangleVertices);
        // Note: using upload heaps to transfer static data like vert buffers is not
        // recommended. Every time the GPU needs it, the upload heap will be marshalled
        // over. Please read up on Default Heap usage. An upload heap is used here for
        // code simplicity and because there are very few verts to actually transfer.
        vertexBuffer = renderingBackEnd->createBuffer(backend::Buffer::UPLOAD, vertexBufferSize);
        vertexBuffer->map();
        vertexBuffer->write(&triangleVertices[0], vertexBufferSize);
        vertexBuffer->unmap();
    }

    void Application::onUpdate() {
    }

    void Application::onRender() {
        auto& swapChain = renderingBackEnd->getSwapChain();
        auto& frameData = *framesData[swapChain->getCurrentFrameIndex()];
        auto& commandList = graphicCommandList[swapChain->getCurrentFrameIndex()];
        auto& pipeline = *pipelines["default"];

        swapChain->acquire(frameData);

        commandList->begin(pipeline);
        swapChain->begin(frameData, *commandList);
        renderingBackEnd->beginRendering(*pipelineResources["default"], pipeline, *commandList);

        commandList->bindVertexBuffer(*vertexBuffer);
        commandList->drawInstanced(3);

        renderingBackEnd->endRendering(*commandList);
        swapChain->end(frameData, *commandList);
        commandList->end();
        renderingBackEnd->getGraphicCommandQueue()->submit(frameData, {commandList});

        swapChain->present(frameData);
        swapChain->nextSwapChain();
    }

    void Application::onDestroy() {
        renderingBackEnd->getDevice()->waitIdle();
        for (uint32_t i = 0; i < backend::SwapChain::FRAMES_IN_FLIGHT; i++) {
            renderingBackEnd->destroyFrameData(*framesData[i]);
        }
    }

    // Helper function for setting the window's title text.
    void Application::setCustomWindowText(LPCWSTR text) {
        std::wstring windowText = title + L": " + text;
        SetWindowText(Win32Application::getHwnd(), windowText.c_str());
    }

    // Generate a simple black and white checkerboard texture.
    std::vector<UINT8> Application::generateTextureData() {
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