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

        const auto triangleVertices = std::vector<Vertex> {
            { { 0.0f, 0.25f * aspectRatio, 0.0f }, { 0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f} },
            { { 0.25f, -0.25f * aspectRatio, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f } },
            { { -0.25f, -0.25f * aspectRatio, 0.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } }
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

        checkerBoardTexture = renderingBackEnd->createImage(
            backend::ImageFormat::R8G8B8A8_SRGB,
            TextureWidth,
            TextureHeight,
            L"CheckerBoardTexture");
        uploadCommandList->upload(*checkerBoardTexture, generateTextureData().data());

        uploadCommandList->end();
        renderingBackEnd->getTransferCommandQueue()->submit({uploadCommandList});
        renderingBackEnd->getTransferCommandQueue()->waitIdle();
        uploadCommandList->cleanup();

        uboBuffer1 = renderingBackEnd->createBuffer(
            backend::Buffer::UNIFORM,
            sizeof(GlobalUBO1),
            1, 256,
            L"UBO1");
        uboBuffer1->map();

        uboBuffer2 = renderingBackEnd->createBuffer(
            backend::Buffer::UNIFORM,
            sizeof(GlobalUBO2),
            1, 256,
            L"UBO2");
        uboBuffer2->map();

        texturesDescriptorLayout = renderingBackEnd->createDescriptorLayout(
            backend::DescriptorType::TEXTURE,
            1,
            staticSamplers,
            L"Textures");
        uniformDescriptorLayout = renderingBackEnd->createDescriptorLayout(
            backend::DescriptorType::BUFFER,
            2,
            L"UBOs");

        pipelineResources["default"] = renderingBackEnd->createPipelineResources(
            { uniformDescriptorLayout, texturesDescriptorLayout },
            staticSamplers,
            L"default");

        const auto defaultVertexInputLayout = renderingBackEnd->createVertexLayout(sizeof(Vertex), vertexAttributes);
        const auto vertexShader = renderingBackEnd->createShaderModule("shaders/shaders1_vert");
        const auto fragmentShader = renderingBackEnd->createShaderModule("shaders/shaders1_frag");
        pipelines["default"] = renderingBackEnd->createPipeline(
            *pipelineResources["default"],
            *defaultVertexInputLayout,
            *vertexShader,
            *fragmentShader,
            L"default");

        for (uint32_t i = 0; i < backend::SwapChain::FRAMES_IN_FLIGHT; i++) {
            auto texturesDescriptorSet = renderingBackEnd->createDescriptorSet(*texturesDescriptorLayout, L"Textures");
            auto uniformDescriptorSet = renderingBackEnd->createDescriptorSet(*uniformDescriptorLayout, L"Uniform");

            auto checkerBoardHandle = texturesDescriptorSet->allocate();
            texturesDescriptorSet->update(checkerBoardHandle, *checkerBoardTexture);
            auto uboHandle1 = uniformDescriptorSet->allocate();
            auto uboHandle2 = uniformDescriptorSet->allocate();
            uniformDescriptorSet->update({uboHandle1, uboHandle2}, {uboBuffer1, uboBuffer2});

            framesData[i] = renderingBackEnd->createFrameData(i, {uniformDescriptorSet, texturesDescriptorSet});
            graphicCommandAllocator[i] = renderingBackEnd->createCommandAllocator(backend::CommandList::GRAPHIC);
            graphicCommandList[i] = graphicCommandAllocator[i]->createCommandList(*pipelines["default"]);
        }
    }

    void Application::onUpdate() {
        const float translationSpeed = 0.005f;
        const float offsetBounds = 1.25f;
        ubo1.offset.x += translationSpeed;
        if (ubo1.offset.x > offsetBounds) {
            ubo1.offset.x = -offsetBounds;
        }
        ubo1.scale += 0.001f * scaleIncrement;
        if ((ubo1.scale > 1.5f) || (ubo1.scale < 0.5f)) {
            scaleIncrement = -scaleIncrement;
        }
        uboBuffer1->write(&ubo1);

        ubo2.color += 0.01f * colorIncrement;
        if ((ubo2.color.x > 0.5f) || (ubo2.color.x < 0.0f)) {
            colorIncrement = -colorIncrement;
        }
        uboBuffer2->write(&ubo2);
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
        uboBuffer1->unmap();
        uboBuffer2->unmap();
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