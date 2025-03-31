// https://github.com/microsoft/DirectX-Graphics-Samples/tree/master/Samples/Desktop/D3D12HelloWorld/src/HelloTriangle
module;
#include "VKLibraries.h"

export module dxvk.app.vulkan;

import dxvk.app;

export namespace dxvk {

    class VKApplication: public BaseApplication {
    public:
        VKApplication(UINT width, UINT height, std::wstring name);

        void OnInit() override;
        void OnUpdate() override;
        void OnRender() override;
        void OnDestroy() override;

    private:
        struct Vertex
        {
            glm::vec3 position;
            glm::vec2 uv;
        };
        struct SceneConstantBuffer
        {
            glm::vec4 offset;
            float padding[60]; // Padding so the constant buffer is 256-byte aligned.
        };
        static constexpr UINT FrameCount = 2;
        SceneConstantBuffer m_constantBufferData;


        void LoadPipeline();

   };

}