// https://github.com/microsoft/DirectX-Graphics-Samples/tree/master/Samples/Desktop/D3D12HelloWorld/src/HelloTriangle
module;
#include "DXLibraries.h"
#include "DXTools.h"

module dxvk.app.directx;

import dxvk.app.win32;
import dxvk.backend.directx;

namespace dxvk {

    DXApplication::DXApplication(UINT width, UINT height, std::wstring name) : Application(width, height, name),
       m_scissorRect(0, 0, static_cast<LONG>(width), static_cast<LONG>(height)),
       m_viewport(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height)),
       m_rtvDescriptorSize(0),
       m_constantBufferData{},
       m_pCbvDataBegin(nullptr)
    {
    }

    void DXApplication::OnInit() {
        LoadPipeline();
        LoadAssets();
    }

    void DXApplication::OnUpdate() {
        const float translationSpeed = 0.005f;
        const float offsetBounds = 1.25f;

        m_constantBufferData.offset.x += translationSpeed;
        if (m_constantBufferData.offset.x > offsetBounds)
        {
            m_constantBufferData.offset.x = -offsetBounds;
        }
        memcpy(m_pCbvDataBegin, &m_constantBufferData, sizeof(m_constantBufferData));
    }

    void DXApplication::OnRender() {
        auto backend = std::static_pointer_cast<backend::DXRenderingBackEnd>(renderingBackEnd);
        auto m_commandQueue = backend->getDXGraphicCommandQueue()->getCommandQueue();

        // Record all the commands we need to render the scene into the command list.
        PopulateCommandList();

        // Execute the command list.
        ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
        m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

        // Present the frame.
    }

    void DXApplication::OnDestroy() {
        // Ensure that the GPU is no longer referencing resources that are about to be
        // cleaned up by the destructor.
        WaitForPreviousFrame();

        CloseHandle(m_fenceEvent);
    }

    void DXApplication::PopulateCommandList() {
        auto backend = std::static_pointer_cast<backend::DXRenderingBackEnd>(renderingBackEnd);
        auto m_frameIndex = backend->getDXSwapChain()->getCurrentFrameIndex();

        // Command list allocators can only be reset when the associated
        // command lists have finished execution on the GPU; apps should use
        // fences to determine GPU execution progress.
        ThrowIfFailed(m_commandAllocator->Reset());

        // However, when ExecuteCommandList() is called on a particular command
        // list, that command list can then be reset at any time and must be before
        // re-recording.
        ThrowIfFailed(m_commandList->Reset(m_commandAllocator.Get(), m_pipelineState.Get()));

        // Set necessary state.
        m_commandList->SetGraphicsRootSignature(m_rootSignature.Get());

        ID3D12DescriptorHeap* ppHeaps[] = { m_srvCbvHeap.Get() };
        m_commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

        D3D12_GPU_DESCRIPTOR_HANDLE gpuHeapHandle = m_srvCbvHeap->GetGPUDescriptorHandleForHeapStart();
        m_commandList->SetGraphicsRootDescriptorTable(ROOT_PARAMETER_INDEX_SRV, gpuHeapHandle);
        D3D12_GPU_DESCRIPTOR_HANDLE srvHandle = { gpuHeapHandle.ptr + m_srvCbvDescriptorSize };
        m_commandList->SetGraphicsRootDescriptorTable(ROOT_PARAMETER_INDEX_CBV, srvHandle);
        m_commandList->RSSetViewports(1, &m_viewport);
        m_commandList->RSSetScissorRects(1, &m_scissorRect);

        // Indicate that the back buffer will be used as a render target.
        auto barrier1 = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(),
                                                  D3D12_RESOURCE_STATE_PRESENT,
                                                  D3D12_RESOURCE_STATE_RENDER_TARGET);
        m_commandList->ResourceBarrier(1, &barrier1);

        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorSize);
        m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

        // Record commands.
        const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
        m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
        m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        m_commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
        m_commandList->DrawInstanced(3, 1, 0, 0);

        // Indicate that the back buffer will now be used to present.
        auto barrier2 = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
        m_commandList->ResourceBarrier(1, &barrier2);

        ThrowIfFailed(m_commandList->Close());

    }

    void DXApplication::LoadAssets() {
        auto backend = std::static_pointer_cast<backend::DXRenderingBackEnd>(renderingBackEnd);
        auto m_device = backend->getDXDevice()->getDevice();
        auto m_commandQueue = backend->getDXGraphicCommandQueue()->getCommandQueue();
        // Create the root signature.
        {

            D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};

            // This is the highest version the sample supports. If CheckFeatureSupport succeeds, the HighestVersion returned will not be greater than this.
            featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

            if (FAILED(m_device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
            {
                featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
            }

            CD3DX12_DESCRIPTOR_RANGE1 ranges[2];
            ranges[ROOT_PARAMETER_INDEX_SRV].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
            ranges[ROOT_PARAMETER_INDEX_CBV].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);

            CD3DX12_ROOT_PARAMETER1 rootParameters[2];
            rootParameters[ROOT_PARAMETER_INDEX_SRV].InitAsDescriptorTable(1, &ranges[ROOT_PARAMETER_INDEX_SRV], D3D12_SHADER_VISIBILITY_PIXEL);
            rootParameters[ROOT_PARAMETER_INDEX_CBV].InitAsDescriptorTable(1, &ranges[ROOT_PARAMETER_INDEX_CBV], D3D12_SHADER_VISIBILITY_VERTEX);

            D3D12_STATIC_SAMPLER_DESC sampler = {};
            sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
            sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
            sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
            sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
            sampler.MipLODBias = 0;
            sampler.MaxAnisotropy = 0;
            sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
            sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
            sampler.MinLOD = 0.0f;
            sampler.MaxLOD = D3D12_FLOAT32_MAX;
            sampler.ShaderRegister = 0;
            sampler.RegisterSpace = 0;
            sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

            // Allow input layout and deny uneccessary access to certain pipeline stages.
            D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
                D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
                D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
                D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
                D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

            CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
            rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 1, &sampler, rootSignatureFlags);

            ComPtr<ID3DBlob> signature;
            ComPtr<ID3DBlob> error;
            ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error));
            ThrowIfFailed(m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)));
        }

        // Create the pipeline state, which includes compiling and loading shaders.
        {
            ComPtr<ID3DBlob> vertexShader;
            ComPtr<ID3DBlob> pixelShader;

    #if defined(_DEBUG)
            // Enable better shader debugging with the graphics debugging tools.
            UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
    #else
            UINT compileFlags = 0;
    #endif

            ThrowIfFailed(D3DCompileFromFile(GetAssetFullPath(L"..\\shaders\\shaders.hlsl").c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr));
            ThrowIfFailed(D3DCompileFromFile(GetAssetFullPath(L"..\\shaders\\shaders.hlsl").c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr));

            // Define the vertex input layout.
            D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
            {
                { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
                { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
            };

            // Describe and create the graphics pipeline state object (PSO).
            D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
            psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
            psoDesc.pRootSignature = m_rootSignature.Get();
            psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
            psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
            psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
            psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
            psoDesc.DepthStencilState.DepthEnable = FALSE;
            psoDesc.DepthStencilState.StencilEnable = FALSE;
            psoDesc.SampleMask = UINT_MAX;
            psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
            psoDesc.NumRenderTargets = 1;
            psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
            psoDesc.SampleDesc.Count = 1;
            ThrowIfFailed(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState)));
        }

        // Create the command list.
        ThrowIfFailed(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), m_pipelineState.Get(), IID_PPV_ARGS(&m_commandList)));

        // Create the vertex buffer.
        {
            // Define the geometry for a triangle.
            Vertex triangleVertices[] =
            {
                { { 0.0f, 0.25f * m_aspectRatio, 0.0f }, { 0.5f, 0.0f } },
                { { 0.25f, -0.25f * m_aspectRatio, 0.0f }, { 1.0f, 1.0f } },
                { { -0.25f, -0.25f * m_aspectRatio, 0.0f }, { 0.0f, 1.0f } }
            };

            const UINT vertexBufferSize = sizeof(triangleVertices);

            // Note: using upload heaps to transfer static data like vert buffers is not
            // recommended. Every time the GPU needs it, the upload heap will be marshalled
            // over. Please read up on Default Heap usage. An upload heap is used here for
            // code simplicity and because there are very few verts to actually transfer.
            auto heapUpload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
            auto bufferSize = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);
            ThrowIfFailed(m_device->CreateCommittedResource(
                &heapUpload,
                D3D12_HEAP_FLAG_NONE,
                &bufferSize,
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&m_vertexBuffer)));

            // Copy the triangle data to the vertex buffer.
            UINT8* pVertexDataBegin;
            CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
            ThrowIfFailed(m_vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)));
            memcpy(pVertexDataBegin, triangleVertices, sizeof(triangleVertices));
            m_vertexBuffer->Unmap(0, nullptr);

            // Initialize the vertex buffer view.
            m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
            m_vertexBufferView.StrideInBytes = sizeof(Vertex);
            m_vertexBufferView.SizeInBytes = vertexBufferSize;
        }

        // Note: ComPtr's are CPU objects but this resource needs to stay in scope until
        // the command list that references it has finished executing on the GPU.
        // We will flush the GPU at the end of this method to ensure the resource is not
        // prematurely destroyed.
        ComPtr<ID3D12Resource> textureUploadHeap;

        auto srvHandle = m_srvCbvHeap->GetCPUDescriptorHandleForHeapStart();
        // Create the texture.
        {
            // Describe and create a Texture2D.
            D3D12_RESOURCE_DESC textureDesc = {};
            textureDesc.MipLevels = 1;
            textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            textureDesc.Width = TextureWidth;
            textureDesc.Height = TextureHeight;
            textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
            textureDesc.DepthOrArraySize = 1;
            textureDesc.SampleDesc.Count = 1;
            textureDesc.SampleDesc.Quality = 0;
            textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

            auto heapDefault = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
            ThrowIfFailed(m_device->CreateCommittedResource(
                &heapDefault,
                D3D12_HEAP_FLAG_NONE,
                &textureDesc,
                D3D12_RESOURCE_STATE_COPY_DEST,
                nullptr,
                IID_PPV_ARGS(&m_texture)));

            const UINT64 uploadBufferSize = GetRequiredIntermediateSize(m_texture.Get(), 0, 1);

            // Create the GPU upload buffer.
            auto heapUpload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
            auto bufferSize = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);
            ThrowIfFailed(m_device->CreateCommittedResource(
                &heapUpload,
                D3D12_HEAP_FLAG_NONE,
                &bufferSize,
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&textureUploadHeap)));

            // Copy data to the intermediate upload heap and then schedule a copy
            // from the upload heap to the Texture2D.
            std::vector<UINT8> texture = GenerateTextureData();

            D3D12_SUBRESOURCE_DATA textureData = {};
            textureData.pData = &texture[0];
            textureData.RowPitch = TextureWidth * TexturePixelSize;
            textureData.SlicePitch = textureData.RowPitch * TextureHeight;

            UpdateSubresources(m_commandList.Get(), m_texture.Get(), textureUploadHeap.Get(), 0, 0, 1, &textureData);
            auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_texture.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
            m_commandList->ResourceBarrier(1, &barrier);

            // Describe and create a SRV for the texture.
            D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
            srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            srvDesc.Format = textureDesc.Format;
            srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MipLevels = 1;
            m_device->CreateShaderResourceView(m_texture.Get(), &srvDesc, srvHandle);
        }

        // Create the constant buffer.
        {
            const UINT constantBufferSize = sizeof(SceneConstantBuffer);    // CB size is required to be 256-byte aligned.

            auto heapUpload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
            auto bufferSize = CD3DX12_RESOURCE_DESC::Buffer(constantBufferSize);
            ThrowIfFailed(m_device->CreateCommittedResource(
                &heapUpload,
                D3D12_HEAP_FLAG_NONE,
                &bufferSize,
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&m_constantBuffer)));

            // Describe and create a constant buffer view.
            D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
            cbvDesc.BufferLocation = m_constantBuffer->GetGPUVirtualAddress();
            cbvDesc.SizeInBytes = constantBufferSize;
            D3D12_CPU_DESCRIPTOR_HANDLE cbvHandle = { srvHandle.ptr + m_srvCbvDescriptorSize };
            m_device->CreateConstantBufferView(&cbvDesc, cbvHandle);

            // Map and initialize the constant buffer. We don't unmap this until the
            // app closes. Keeping things mapped for the lifetime of the resource is okay.
            CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
            ThrowIfFailed(m_constantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&m_pCbvDataBegin)));
            // m_constantBufferData.offset = XMFLOAT4(0.5, 0.5, 0.5, 0.5);
            memcpy(m_pCbvDataBegin, &m_constantBufferData, sizeof(m_constantBufferData));
        }

        // Close the command list and execute it to begin the initial GPU setup.
        ThrowIfFailed(m_commandList->Close());
        ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
        m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

        // Create synchronization objects and wait until assets have been uploaded to the GPU.
        {


            // Wait for the command list to execute; we are reusing the same command
            // list in our main loop but for now, we just want to wait for setup to
            // complete before continuing.
            WaitForPreviousFrame();
        }
    }

    void DXApplication::WaitForPreviousFrame() {

    }

    void DXApplication::LoadPipeline() {
        // Create descriptor heaps.
        {
            // TEXTURE : Describe and create a shader resource view (SRV) heap for the texture.
            // Describe and create a constant buffer view (CBV) descriptor heap.
            // Flags indicate that this descriptor heap can be bound to the pipeline
            // and that descriptors contained in it can be referenced by a root table.
            D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
            srvHeapDesc.NumDescriptors = 2;
            srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
            ThrowIfFailed(device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_srvCbvHeap)));
            m_srvCbvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

        }


        ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)));
    }


}