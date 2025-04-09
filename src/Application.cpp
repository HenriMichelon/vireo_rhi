/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "Tools.h"

module vireo.app;

import vireo.app.win32;
import vireo.backend.directx;
import vireo.backend.vulkan;

namespace vireo {

    Application::Application(UINT width, UINT height, const std::wstring& name) :
        title(name),
        width(width),
        height(height) {
        aspectRatio = static_cast<float>(width) / static_cast<float>(height);
        renderingBackEnd = std::make_shared<backend::VKRenderingBackEnd>(width, height);
        // renderingBackEnd = std::make_shared<backend::DXRenderingBackEnd>(width, height);
    }

    void Application::onInit() {
        renderingBackEnd->setClearColor({ 0.0f, 0.2f, 0.4f, 1.0f });
        const auto triangleVertices = std::vector<Vertex> {
                { { 0.0f, 0.25f * aspectRatio, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
                { { 0.25f, -0.25f * aspectRatio, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
                { { -0.25f, -0.25f * aspectRatio, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
        };
        const auto uploadCommandAllocator = renderingBackEnd->createCommandAllocator(backend::CommandList::TRANSFER);
        auto uploadCommandList = uploadCommandAllocator->createCommandList();
        uploadCommandList->reset();
        uploadCommandList->begin();
        vertexBuffer = renderingBackEnd->createBuffer(
            backend::Buffer::Type::VERTEX,
            sizeof(Vertex),
            triangleVertices.size(),
            1,
            L"TriangleVertexBuffer");
        uploadCommandList->upload(*vertexBuffer, &triangleVertices[0]);
        uploadCommandList->end();
        renderingBackEnd->getTransferCommandQueue()->submit({uploadCommandList});

        const auto attributes = std::vector{
            backend::VertexInputLayout::AttributeDescription{"POSITION", backend::VertexInputLayout::R32G32B32_FLOAT, 0},
            backend::VertexInputLayout::AttributeDescription{"COLOR",    backend::VertexInputLayout::R32G32B32A32_FLOAT, 12}
        };
        const auto defaultVertexInputLayout = renderingBackEnd->createVertexLayout(sizeof(Vertex), attributes);
        const auto vertexShader = renderingBackEnd->createShaderModule("shaders/shaders1_vert");
        const auto fragmentShader = renderingBackEnd->createShaderModule("shaders/shaders1_frag");
        pipelineResources["default"] = renderingBackEnd->createPipelineResources();
        pipelines["default"] = renderingBackEnd->createPipeline(
            *pipelineResources["default"],
            *defaultVertexInputLayout,
            *vertexShader,
            *fragmentShader);

        for (uint32_t i = 0; i < backend::SwapChain::FRAMES_IN_FLIGHT; i++) {
            framesData[i] = renderingBackEnd->createFrameData(i);
            graphicCommandAllocator[i] = renderingBackEnd->createCommandAllocator(backend::CommandList::GRAPHIC);
            graphicCommandList[i] = graphicCommandAllocator[i]->createCommandList(*pipelines["default"]);
        }

        renderingBackEnd->getTransferCommandQueue()->waitIdle();
        uploadCommandList->cleanup();
    }

    void Application::onUpdate() {
    }

    void Application::onRender() {
        auto& swapChain = renderingBackEnd->getSwapChain();
        auto& frameData = *framesData[swapChain->getCurrentFrameIndex()];
        auto& commandList = graphicCommandList[swapChain->getCurrentFrameIndex()];
        auto& pipeline = *pipelines["default"];

        swapChain->acquire(frameData);

        commandList->reset();
        commandList->begin(pipeline);
        swapChain->begin(frameData, *commandList);
        renderingBackEnd->beginRendering(frameData, *pipelineResources["default"], pipeline, *commandList);

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
        renderingBackEnd->waitIdle();
        for (uint32_t i = 0; i < backend::SwapChain::FRAMES_IN_FLIGHT; i++) {
            renderingBackEnd->destroyFrameData(*framesData[i]);
        }
    }

    // Helper function for setting the window's title text.
    void Application::setCustomWindowText(LPCWSTR text) const {
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