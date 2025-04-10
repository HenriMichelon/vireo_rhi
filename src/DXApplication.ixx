// https://github.com/microsoft/DirectX-Graphics-Samples/tree/master/Samples/Desktop/D3D12HelloWorld/src/HelloTriangle

    class DXApplication: public BaseApplication {
        ComPtr<ID3D12DescriptorHeap> m_srvCbvHeap;
        UINT m_srvCbvDescriptorSize;
        ComPtr<ID3D12Resource> m_texture;

    };

