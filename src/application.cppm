// https://github.com/microsoft/DirectX-Graphics-Samples/tree/master/Samples/Desktop/D3D12HelloWorld/src/HelloTriangle
module;
#include "libraries.h"
#include "DXSample.h"

export module dx12.app;

export namespace dx12 {

    LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

    class Application: public DXSample {
    public:
        Application(UINT width, UINT height, std::wstring name);

        virtual void OnInit();
        virtual void OnUpdate();
        virtual void OnRender();
        virtual void OnDestroy();

    private:
        struct Vertex
        {
            XMFLOAT3 position;
            // XMFLOAT4 color;
            XMFLOAT2 uv;
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
        ComPtr<ID3D12DescriptorHeap> m_srvHeap;
        ComPtr<ID3D12Resource> m_texture;
        std::vector<UINT8> GenerateTextureData();
    };

}