/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "Tools.h"

export module vireo.app;

import vireo.backend;

export namespace vireo {

    class Application final {
    public:
        Application(backend::RenderingBackends& backendType, const std::wstring& name);

        void onInit();
        void onUpdate();
        void onRender();
        void onDestroy();

        void onKeyDown(uint32_t /*key*/)   {}
        void onKeyUp(uint32_t /*key*/)     {}

    protected:
        static constexpr auto TextureWidth = 256;
        static constexpr auto TextureHeight = 256;
        static constexpr auto TexturePixelSize = 4;
        std::vector<unsigned char> generateTextureData() const;

    private:
        struct Vertex {
            glm::vec3 pos;
            glm::vec2 uv;
            // glm::vec4 color;
        };
        struct GlobalUBO1 {
            glm::vec4 offset;
            alignas(16) float scale{1.0f};
        };
        GlobalUBO1 ubo1{};
        struct GlobalUBO2 {
            glm::vec3 color;
        };
        GlobalUBO2 ubo2{};

        std::wstring title;
        std::unique_ptr<backend::RenderingBackEnd> renderingBackEnd;
        std::vector<std::shared_ptr<backend::FrameData>> framesData{backend::SwapChain::FRAMES_IN_FLIGHT};
        std::vector<std::shared_ptr<backend::CommandAllocator>> graphicCommandAllocator{backend::SwapChain::FRAMES_IN_FLIGHT};
        std::vector<std::shared_ptr<backend::CommandList>> graphicCommandList{backend::SwapChain::FRAMES_IN_FLIGHT};
        std::shared_ptr<backend::DescriptorSet> uniformDescriptorSet;
        std::shared_ptr<backend::DescriptorSet> texturesDescriptorSet;
        std::vector<std::shared_ptr<backend::Sampler>> staticSamplers;
        std::map<std::string, std::shared_ptr<backend::PipelineResources>> pipelineResources;
        std::map<std::string, std::shared_ptr<backend::Pipeline>> pipelines;
        std::shared_ptr<backend::Buffer> vertexBuffer;
        std::shared_ptr<backend::Buffer> uboBuffer1;
        std::shared_ptr<backend::Buffer> uboBuffer2;
        backend::DescriptorHandle uboHandle1;
        backend::DescriptorHandle uboHandle2;
        backend::DescriptorHandle checkerBoardHandle;
        float colorIncrement{1.0f};
        float scaleIncrement{1.0f};
        std::shared_ptr<backend::Image> checkerBoardTexture;

    };
}