// https://github.com/microsoft/DirectX-Graphics-Samples/tree/master/Samples/Desktop/D3D12HelloWorld/src/HelloTriangle

    class DXApplication: public BaseApplication {
        ComPtr<ID3D12DescriptorHeap> m_srvCbvHeap;
        UINT m_srvCbvDescriptorSize;
        ComPtr<ID3D12Resource> m_texture;

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

