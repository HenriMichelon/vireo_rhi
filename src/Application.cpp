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

    Application::Application(backend::RenderingBackends& backendType, const std::wstring& name) :
        title(name) {
        if (backendType == backend::RenderingBackends::VULKAN) {
            renderingBackEnd = std::make_unique<backend::VKRenderingBackEnd>();
        } else {
            renderingBackEnd = std::make_unique<backend::DXRenderingBackEnd>();
        }
    }

    void Application::onInit() {
        renderingBackEnd->setClearColor({ 0.0f, 0.2f, 0.4f, 1.0f });
        const auto aspectRatio = renderingBackEnd->getSwapChain()->getAspectRatio();

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

        uniformDescriptorSet = renderingBackEnd->createDescriptorSet(backend::DescriptorType::BUFFER, 100);
        sceneConstantBufferHandle =  uniformDescriptorSet->allocate();
        sceneConstantBuffer = renderingBackEnd->createBuffer(
            backend::Buffer::UNIFORM,
            sizeof(SceneConstantBuffer),
            1, 256,
            L"ConstantBuffer");
        uniformDescriptorSet->update(sceneConstantBufferHandle, *sceneConstantBuffer);

        staticSamplers.push_back(renderingBackEnd->createSampler(
            backend::Filter::NEAREST,
            backend::Filter::NEAREST,
            backend::AddressMode::CLAMP_TO_BORDER,
            backend::AddressMode::CLAMP_TO_BORDER,
            backend::AddressMode::CLAMP_TO_BORDER,
            0.0f, 1.0f,
            false,
            backend::MipMapMode::NEAREST));
        staticSamplers.push_back(renderingBackEnd->createSampler(
            backend::Filter::LINEAR,
            backend::Filter::LINEAR,
            backend::AddressMode::CLAMP_TO_BORDER,
            backend::AddressMode::CLAMP_TO_BORDER,
            backend::AddressMode::CLAMP_TO_BORDER,
            0.0f, 1.0f,
            false,
            backend::MipMapMode::LINEAR));
        pipelineResources["default"] = renderingBackEnd->createPipelineResources(
            staticSamplers,
            L"default");

        const auto attributes = std::vector{
            backend::VertexInputLayout::AttributeDescription{"POSITION", backend::VertexInputLayout::R32G32B32_FLOAT, 0},
            backend::VertexInputLayout::AttributeDescription{"COLOR",    backend::VertexInputLayout::R32G32B32A32_FLOAT, 12}
        };
        const auto defaultVertexInputLayout = renderingBackEnd->createVertexLayout(sizeof(Vertex), attributes);
        const auto vertexShader = renderingBackEnd->createShaderModule("shaders/shaders1_vert");
        const auto fragmentShader = renderingBackEnd->createShaderModule("shaders/shaders1_frag");
        pipelines["default"] = renderingBackEnd->createPipeline(
            *pipelineResources["default"],
            *defaultVertexInputLayout,
            *vertexShader,
            *fragmentShader,
            L"default");

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

        if (!swapChain->acquire(frameData)) { return; }

        auto& commandList = graphicCommandList[swapChain->getCurrentFrameIndex()];
        auto& pipeline = *pipelines["default"];

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
        uniformDescriptorSet->free(sceneConstantBufferHandle);
        renderingBackEnd->waitIdle();
        for (uint32_t i = 0; i < backend::SwapChain::FRAMES_IN_FLIGHT; i++) {
            renderingBackEnd->destroyFrameData(*framesData[i]);
        }
    }

    // Generate a simple black and white checkerboard texture.
    // https://github.com/microsoft/DirectX-Graphics-Samples/blob/master/Samples/Desktop/D3D12HelloWorld/src/HelloTexture/D3D12HelloTexture.cpp
    std::vector<unsigned char> Application::generateTextureData() const {
        const auto rowPitch = TextureWidth * TexturePixelSize;
        const auto cellPitch = rowPitch >> 3;        // The width of a cell in the checkboard texture.
        const auto cellHeight = TextureWidth >> 3;    // The height of a cell in the checkerboard texture.
        const auto textureSize = rowPitch * TextureHeight;

        std::vector<UINT8> data(textureSize);
        unsigned char* pData = &data[0];

        for (int n = 0; n < textureSize; n += TexturePixelSize) {
            const auto x = n % rowPitch;
            const auto y = n / rowPitch;
            const auto i = x / cellPitch;
            const auto j = y / cellHeight;

            if (i % 2 == j % 2) {
                pData[n] = 0x00;        // R
                pData[n + 1] = 0x00;    // G
                pData[n + 2] = 0x00;    // B
                pData[n + 3] = 0xff;    // A
            }
            else {
                pData[n] = 0xff;        // R
                pData[n + 1] = 0xff;    // G
                pData[n + 2] = 0xff;    // B
                pData[n + 3] = 0xff;    // A
            }
        }

        return data;
    }

}