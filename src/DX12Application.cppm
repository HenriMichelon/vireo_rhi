// https://github.com/microsoft/DirectX-Graphics-Samples/tree/master/Samples/Desktop/D3D12HelloWorld/src/HelloTriangle
module;
#include "DX12Libraries.h"

export module dxvk.app;

import dxvk.baseapp;

export namespace dxvk {

    class DX12Application: public BaseApplication {
    public:
        DX12Application(UINT width, UINT height, std::wstring name);

        void OnInit() override;
        void OnUpdate() override;
        void OnRender() override;
        void OnDestroy() override;

    private:
        struct Vertex
        {
            glm::vec3 position;
            // ve4 color;
            glm::vec2 uv;
        };

        // Triangle
        static constexpr UINT FrameCount = 2;
        ComPtr<ID3D12Device> m_device;
        ComPtr<ID3D12CommandQueue> m_commandQueue;
        ComPtr<IDXGISwapChain3> m_swapChain;
        UINT m_frameIndex;
        ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
        UINT m_rtvDescriptorSize;
        ComPtr<ID3D12Resource> m_renderTargets[FrameCount];
        ComPtr<ID3D12CommandAllocator> m_commandAllocator;
        ComPtr<ID3D12RootSignature> m_rootSignature;
        ComPtr<ID3D12PipelineState> m_pipelineState;
        ComPtr<ID3D12GraphicsCommandList> m_commandList;
        ComPtr<ID3D12Resource> m_vertexBuffer;
        D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
        HANDLE m_fenceEvent;
        ComPtr<ID3D12Fence> m_fence;
        UINT64 m_fenceValue;
        CD3DX12_VIEWPORT m_viewport;
        CD3DX12_RECT m_scissorRect;
        void LoadPipeline();
        void LoadAssets();
        void WaitForPreviousFrame();
        void PopulateCommandList();

        // Texture
        static constexpr  UINT TextureWidth = 256;
        static constexpr  UINT TextureHeight = 256;
        static constexpr  UINT TexturePixelSize = 4;    // The number of bytes used to represent a pixel in the texture.
        ComPtr<ID3D12DescriptorHeap> m_srvCbvHeap;
        UINT m_srvCbvDescriptorSize;
        ComPtr<ID3D12Resource> m_texture;
        std::vector<UINT8> GenerateTextureData();

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