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
            glm::vec4 color;
        };
        struct SceneConstantBuffer {
            glm::vec4 offset;
        };

        std::wstring title;
        std::unique_ptr<backend::RenderingBackEnd> renderingBackEnd;
        std::vector<std::shared_ptr<backend::FrameData>> framesData{backend::SwapChain::FRAMES_IN_FLIGHT};
        std::vector<std::shared_ptr<backend::CommandAllocator>> graphicCommandAllocator{backend::SwapChain::FRAMES_IN_FLIGHT};
        std::vector<std::shared_ptr<backend::CommandList>> graphicCommandList{backend::SwapChain::FRAMES_IN_FLIGHT};
        std::shared_ptr<backend::DescriptorSet> uniformDescriptorSet;
        std::shared_ptr<backend::Sampler> nearestBorderSampler;
        std::shared_ptr<backend::Sampler> linearBorderSampler;
        std::map<std::string, std::shared_ptr<backend::PipelineResources>> pipelineResources;
        std::map<std::string, std::shared_ptr<backend::Pipeline>> pipelines;
        std::shared_ptr<backend::Buffer> vertexBuffer;
        std::shared_ptr<backend::Buffer> sceneConstantBuffer;
        backend::DescriptorHandle sceneConstantBufferHandle;

    };
}