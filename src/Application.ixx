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
import vireo.backend.framedata;

export namespace vireo {

    class Application
    {
    public:
        Application(backend::RenderingBackends& backendType, const std::wstring& name);
        virtual ~Application() = default;

        virtual void onInit();
        virtual void onUpdate();
        virtual void onRender();
        virtual void onDestroy();

        virtual void onKeyDown(UINT8 /*key*/)   {}
        virtual void onKeyUp(UINT8 /*key*/)     {}

        auto getTitle() const   { return title.c_str(); }

    protected:
        static constexpr  UINT TextureWidth = 256;
        static constexpr  UINT TextureHeight = 256;
        static constexpr  UINT TexturePixelSize = 4;    // The number of bytes used to represent a pixel in the texture.
        std::vector<UINT8> generateTextureData();

    private:
        struct Vertex {
            glm::vec3 pos;
            glm::vec4 color;
        };

        std::wstring title;

        std::shared_ptr<backend::RenderingBackEnd> renderingBackEnd;
        std::vector<std::shared_ptr<backend::FrameData>> framesData{backend::SwapChain::FRAMES_IN_FLIGHT};
        std::vector<std::shared_ptr<backend::CommandAllocator>> graphicCommandAllocator{backend::SwapChain::FRAMES_IN_FLIGHT};
        std::vector<std::shared_ptr<backend::CommandList>> graphicCommandList{backend::SwapChain::FRAMES_IN_FLIGHT};
        std::map<std::string, std::shared_ptr<backend::PipelineResources>> pipelineResources;
        std::map<std::string, std::shared_ptr<backend::Pipeline>> pipelines;
        std::shared_ptr<backend::Buffer> vertexBuffer;

        void setCustomWindowText(LPCWSTR text) const;
    };
}