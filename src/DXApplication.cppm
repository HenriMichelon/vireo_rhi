// https://github.com/microsoft/DirectX-Graphics-Samples/tree/master/Samples/Desktop/D3D12HelloWorld/src/HelloTriangle
module;
#include "DXLibraries.h"

export module dxvk.app.directx;

import dxvk.app;
import dxvk.backend;

export namespace dxvk {

    class DXApplication: public BaseApplication {
    public:
        DXApplication(UINT width, UINT height, std::wstring name);

        void OnUpdate() override;
        void OnRender() override;

    private:


        // Triangle
        ComPtr<ID3D12Resource> m_vertexBuffer;
        D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
        HANDLE m_fenceEvent;
        ComPtr<ID3D12Fence> m_fence;
        UINT64 m_fenceValue;
        CD3DX12_VIEWPORT m_viewport;
        CD3DX12_RECT m_scissorRect;
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