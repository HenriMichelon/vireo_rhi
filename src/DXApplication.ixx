// https://github.com/microsoft/DirectX-Graphics-Samples/tree/master/Samples/Desktop/D3D12HelloWorld/src/HelloTriangle
module;
#include "DXTools.h"

export module vireo.app.directx;

import vireo.app;
import vireo.backend;

export namespace dxvk {

    class DXApplication: public BaseApplication {
    public:
        DXApplication(UINT width, UINT height, std::wstring name);

        void OnUpdate() override;
        void OnRender() override;

    private:


        // Triangle
        void LoadPipeline();
        void LoadAssets();
        void PopulateCommandList();

        ComPtr<ID3D12DescriptorHeap> m_srvCbvHeap;
        UINT m_srvCbvDescriptorSize;
        ComPtr<ID3D12Resource> m_texture;

        // Buffer
        enum rootParameterIndex {
          ROOT_PARAMETER_INDEX_SRV = 0,
          ROOT_PARAMETER_INDEX_CBV = 1,
        };
        struct SceneConstantBuffer
        {
            XMFLOAT4 offset;
            float padding[60]; // Padding so the constant buffer is 256-byte aligned.
        };
        static_assert((sizeof(SceneConstantBuffer) % 256) == 0, "Constant Buffer size must be 256-byte aligned");
        ComPtr<ID3D12Resource> m_constantBuffer;
        SceneConstantBuffer m_constantBufferData;
        UINT8* m_pCbvDataBegin;
    };

}