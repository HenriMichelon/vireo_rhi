module;
#include "DXTools.h"

module dxvk.backend.directx;

import dxvk.app.win32;
import dxvk.backend.directx.framedata;

namespace dxvk::backend {

    DXRenderingBackEnd::DXRenderingBackEnd(uint32_t width, uint32_t height):
        scissorRect(0, 0, static_cast<LONG>(width), static_cast<LONG>(height)),
        viewport(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height)){
        instance = std::make_shared<DXInstance>();
        physicalDevice = std::make_shared<DXPhysicalDevice>(getDXInstance()->getFactory());
        device = std::make_shared<DXDevice>(getDXPhysicalDevice()->getHardwareAdapater());
        graphicCommandQueue = std::make_shared<DXSubmitQueue>(getDXDevice()->getDevice());
        transferCommandQueue = graphicCommandQueue; // TODO multiple queues for threading
        presentCommandQueue = graphicCommandQueue; // TODO multiple queues for threading
        swapChain = std::make_shared<DXSwapChain>(
            getDXInstance()->getFactory(),
            *getDXDevice(),
            getDXGraphicCommandQueue()->getCommandQueue(),
            width, height);
    }

    std::shared_ptr<FrameData> DXRenderingBackEnd::createFrameData(uint32_t frameIndex) {
        return std::make_shared<DXFrameData>();
    }

    std::shared_ptr<PipelineResources> DXRenderingBackEnd::createPipelineResources() const {
        return std::make_shared<DXPipelineResources>(getDXDevice()->getDevice());
    }

    std::shared_ptr<Pipeline> DXRenderingBackEnd::createPipeline(
            PipelineResources& pipelineResources,
            VertexInputLayout& vertexInputLayout,
            ShaderModule& vertexShader,
            ShaderModule& fragmentShader) const {
        return std::make_shared<DXPipeline>(getDXDevice()->getDevice(), pipelineResources, vertexInputLayout, vertexShader, fragmentShader);
    }

    std::shared_ptr<VertexInputLayout> DXRenderingBackEnd::createVertexLayout(
        size_t,
        const std::vector<VertexInputLayout::AttributeDescription>& attributesDescriptions) const {
        return std::make_shared<DXVertexInputLayout>(attributesDescriptions);
    }

    std::shared_ptr<ShaderModule> DXRenderingBackEnd::createShaderModule(const std::string& fileName) const {
        return std::make_shared<DXShaderModule>(fileName);
    }

    std::shared_ptr<Buffer> DXRenderingBackEnd::createBuffer(Buffer::Type type, size_t size, size_t count) const {
        return make_shared<DXBuffer>(getDXDevice()->getDevice(), type, size, count);
    }

    void DXRenderingBackEnd::beginRendering(PipelineResources& pipelineResources, Pipeline& pipeline, CommandList& commandList) {
        auto dxCommandList = static_cast<DXCommandList&>(commandList).getCommandList();
        auto dxSwapChain = getDXSwapChain();
        auto frameIndex = swapChain->getCurrentFrameIndex();
        auto& dxPipelineResources = static_cast<DXPipelineResources&>(pipelineResources);
        // auto& dxPipeline = static_cast<DXPipeline&>(pipeline);

        dxCommandList->SetGraphicsRootSignature(dxPipelineResources.getRootSignature().Get());
        dxCommandList->RSSetViewports(1, &viewport);
        dxCommandList->RSSetScissorRects(1, &scissorRect);

        auto swapChainBarrier = CD3DX12_RESOURCE_BARRIER::Transition(dxSwapChain->getRenderTargets()[frameIndex].Get(),
                                                 D3D12_RESOURCE_STATE_PRESENT,
                                                 D3D12_RESOURCE_STATE_RENDER_TARGET);
        dxCommandList->ResourceBarrier(1, &swapChainBarrier);

        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(
            dxSwapChain->getHeap()->GetCPUDescriptorHandleForHeapStart(),
            frameIndex,
            dxSwapChain->getDescriptorSize());
        dxCommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

        const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
        dxCommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
        dxCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    }

    void DXRenderingBackEnd::endRendering(CommandList& commandList) {
        auto dxCommandList = static_cast<DXCommandList&>(commandList).getCommandList();
        auto dxSwapChain = getDXSwapChain();
        auto frameIndex = swapChain->getCurrentFrameIndex();

        auto swapChainBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
            dxSwapChain->getRenderTargets()[frameIndex].Get(),
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            D3D12_RESOURCE_STATE_PRESENT);
        dxCommandList->ResourceBarrier(1, &swapChainBarrier);
    }

    DXInstance::DXInstance() {
        UINT dxgiFactoryFlags = 0;
#if defined(_DEBUG)
        {
            ComPtr<ID3D12Debug> debugController;
            if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
                debugController->EnableDebugLayer();
                dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
            }
        }
#endif
        ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));
        // This sample does not support fullscreen transitions.
        ThrowIfFailed(factory->MakeWindowAssociation(Win32Application::getHwnd(), DXGI_MWA_NO_ALT_ENTER));
    }

    DXPhysicalDevice::DXPhysicalDevice(ComPtr<IDXGIFactory4> factory) {
        ComPtr<IDXGIAdapter1> hardwareAdapter;

        SIZE_T maxDedicatedVideoMemory = 0;
        for (UINT i = 0; factory->EnumAdapters1(i, &hardwareAdapter) != DXGI_ERROR_NOT_FOUND; ++i) {
            DXGI_ADAPTER_DESC1 dxgiAdapterDesc1;
            hardwareAdapter->GetDesc1(&dxgiAdapterDesc1);

            // Check to see if the adapter can create a D3D12 device without actually
            // creating it. The adapter with the largest dedicated video memory
            // is favored.
            if ((dxgiAdapterDesc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0 &&
                SUCCEEDED(D3D12CreateDevice(hardwareAdapter.Get(),
                    D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr)) &&
                dxgiAdapterDesc1.DedicatedVideoMemory > maxDedicatedVideoMemory )
            {
                maxDedicatedVideoMemory = dxgiAdapterDesc1.DedicatedVideoMemory;
                ThrowIfFailed(hardwareAdapter.As(&hardwareAdapter4));
            }
        }
        DXGI_ADAPTER_DESC3 desc;
        const HRESULT hr = hardwareAdapter4->GetDesc3(&desc);
        if (SUCCEEDED(hr)) {
            std::wstring adapterName = desc.Description;
            std::wcout << L"Display Adapter Name: " << adapterName << std::endl;
        } else {
            std::cerr << "Failed to get adapter description." << std::endl;
        }
    }

    DXDevice::DXDevice(ComPtr<IDXGIAdapter4> hardwareAdapter4) {
        ThrowIfFailed(
            D3D12CreateDevice(
                  hardwareAdapter4.Get(),
                  D3D_FEATURE_LEVEL_11_0,
                  IID_PPV_ARGS(&device)
          ));
#if defined(_DEBUG)
        ComPtr<ID3D12InfoQueue> infoQueue;
        if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
            infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
            infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
            infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);
        }
#endif
        ThrowIfFailed(device->CreateFence(
            0,
            D3D12_FENCE_FLAG_NONE,
            IID_PPV_ARGS(&inFlightFence)));
        inFlightFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (inFlightFenceEvent == nullptr) {
            ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
        }
    }

    DXDevice::~DXDevice() {
        CloseHandle(inFlightFenceEvent);
    }

    std::shared_ptr<CommandAllocator> DXDevice::createCommandAllocator(CommandAllocator::Type type) const {
        return std::make_shared<DXCommandAllocator>(device);
    }

    DXSubmitQueue::DXSubmitQueue(ComPtr<ID3D12Device> device) {
        // Describe and create the command queue.
        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        ThrowIfFailed(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue)));
    }

    void DXSubmitQueue::submit(const FrameData& frameData, std::vector<std::shared_ptr<CommandList>> commandLists) {
        auto dxCommandLists = std::vector<ID3D12CommandList*>(commandLists.size());
        for (int i = 0; i < commandLists.size(); i++) {
            dxCommandLists[i] = static_pointer_cast<DXCommandList>(commandLists[i])->getCommandList().Get();
        }
        commandQueue->ExecuteCommandLists(dxCommandLists.size(), dxCommandLists.data());
    }

    DXCommandAllocator::DXCommandAllocator(ComPtr<ID3D12Device> device):
        device{device} {
        ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator)));
    }

    std::shared_ptr<CommandList> DXCommandAllocator::createCommandList(Pipeline& pipeline) const {
        return std::make_shared<DXCommandList>(device, commandAllocator, static_cast<DXPipeline&>(pipeline).getPipelineState());
    }

    DXCommandList::DXCommandList(ComPtr<ID3D12Device> device, ComPtr<ID3D12CommandAllocator> commandAllocator, ComPtr<ID3D12PipelineState> pipelineState):
        device{device},
        commandAllocator{commandAllocator} {
        ThrowIfFailed(device->CreateCommandList(
            0,
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            commandAllocator.Get(),
            pipelineState.Get(),
            IID_PPV_ARGS(&commandList)));
        ThrowIfFailed(commandList->Close());
    }

    void DXCommandList::begin(Pipeline& pipeline) {
        auto& dxPipeline = static_cast<DXPipeline&>(pipeline);
        ThrowIfFailed(commandAllocator->Reset());
        ThrowIfFailed(commandList->Reset(commandAllocator.Get(), dxPipeline.getPipelineState().Get()));
    }

    void DXCommandList::end() {
        ThrowIfFailed(commandList->Close());
    }

    void DXCommandList::bindVertexBuffer(Buffer& buffer) {
        auto& vertexBuffer = static_cast<DXBuffer&>(buffer);
        commandList->IASetVertexBuffers(0, 1, &vertexBuffer.getBufferView());
    }

    void DXCommandList::drawInstanced(uint32_t vertexCountPerInstance, uint32_t instanceCount) {
        commandList->DrawInstanced(vertexCountPerInstance, instanceCount, 0, 0);
    }

    DXSwapChain::DXSwapChain(
        ComPtr<IDXGIFactory4> factory,
        DXDevice& dxdevice,
        ComPtr<ID3D12CommandQueue> commandQueue,
        uint32_t width,
        uint32_t height) :
        device{dxdevice},
        presentCommandQueue{commandQueue} {
        extent = { width, height };

        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.BufferCount = FRAMES_IN_FLIGHT;
        swapChainDesc.Width = width;
        swapChainDesc.Height = height;
        swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapChainDesc.SampleDesc.Count = 1;

        ComPtr<IDXGISwapChain1> swapChain1;
        ThrowIfFailed(factory->CreateSwapChainForHwnd(
            commandQueue.Get(),        // Swap chain needs the queue so that it can force a flush on it.
            Win32Application::getHwnd(),
            &swapChainDesc,
            nullptr,
            nullptr,
            &swapChain1
        ));
        ThrowIfFailed(swapChain1.As(&swapChain));
        currentFrameIndex = swapChain->GetCurrentBackBufferIndex();

        // Describe and create a render target view (RTV) descriptor heap.
        {
            D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
            rtvHeapDesc.NumDescriptors = FRAMES_IN_FLIGHT;
            rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            ThrowIfFailed(device.getDevice()->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap)));
            rtvDescriptorSize = device.getDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        }

        // Create frame resources.
        {
            CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());
            // Create a RTV for each frame.
            for (UINT n = 0; n < FRAMES_IN_FLIGHT; n++) {
                ThrowIfFailed(swapChain->GetBuffer(n, IID_PPV_ARGS(&renderTargets[n])));
                renderTargets[n]->SetName(L"BackBuffer");
                device.getDevice()->CreateRenderTargetView(renderTargets[n].Get(), nullptr, rtvHandle);
                rtvHandle.Offset(1, rtvDescriptorSize);
            }
        }
    }

    void DXSwapChain::nextSwapChain() {
        currentFrameIndex = swapChain->GetCurrentBackBufferIndex();
        assert(currentFrameIndex < FRAMES_IN_FLIGHT);
    }

    void DXSwapChain::acquire(FrameData& frameData) {
        auto& data = static_cast<DXFrameData&>(frameData);
        const auto currentFenceValue = data.inFlightFenceValue;
        ThrowIfFailed(presentCommandQueue->Signal(device.getInFlightFence().Get(), currentFenceValue));
        // If the next frame is not ready to be rendered yet, wait until it is ready.
        if (device.getInFlightFence()->GetCompletedValue() < currentFenceValue) {
            ThrowIfFailed(device.getInFlightFence()->SetEventOnCompletion(
                currentFenceValue,
                device.getInFlightFenceEvent()
            ));
            WaitForSingleObjectEx(device.getInFlightFenceEvent(), INFINITE, FALSE);
        }
    }

    void DXSwapChain::present(FrameData& frameData) {
        auto& data = static_cast<DXFrameData&>(frameData);
        ThrowAndPrintIfFailed(swapChain->Present(1, 0), device.getDevice().Get());
        data.inFlightFenceValue += 1;
    }

    void DXSwapChain::terminate(FrameData& frameData) {
        const auto& data = static_cast<DXFrameData&>(frameData);
        const auto currentFenceValue = data.inFlightFenceValue;
        ThrowIfFailed(presentCommandQueue->Signal(device.getInFlightFence().Get(), currentFenceValue));
        if (device.getInFlightFence()->GetCompletedValue() < currentFenceValue) {
            ThrowIfFailed(device.getInFlightFence()->SetEventOnCompletion(
                currentFenceValue,
                device.getInFlightFenceEvent()
            ));
            WaitForSingleObjectEx(device.getInFlightFenceEvent(), INFINITE, FALSE);
        }
    }

    DXVertexInputLayout::DXVertexInputLayout(const std::vector<AttributeDescription>& attributesDescriptions) {
        for (int i = 0; i < attributesDescriptions.size(); i++) {
            inputElementDescs.push_back({
                .SemanticName = reinterpret_cast<LPCSTR>(attributesDescriptions[i].binding.c_str()),
                .SemanticIndex = 0,
                .Format = DXFormat[attributesDescriptions[i].format],
                .InputSlot = 0,
                .AlignedByteOffset = attributesDescriptions[i].offset,
                .InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
                .InstanceDataStepRate = 0
            });
        }
    }

    DXShaderModule::DXShaderModule(const std::string& fileName) {
        std::ifstream shaderFile(fileName + ".dxil", std::ios::binary | std::ios::ate);
        if (!shaderFile) {
            die("Error loading shader " + fileName);
        }
        std::streamsize size = shaderFile.tellg();
        shaderFile.seekg(0, std::ios::beg);
        if (FAILED(D3DCreateBlob(size, &shader))) {
            die("Error creating blob for  shader " + fileName);
        }
        shaderFile.read(static_cast<char*>(shader->GetBufferPointer()), size);
    }

    DXPipelineResources::DXPipelineResources(const ComPtr<ID3D12Device>& device) {
        CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
        rootSignatureDesc.Init(
            0,
            nullptr,
            0,
            nullptr,
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
        ComPtr<ID3DBlob> signature;
        ComPtr<ID3DBlob> error;
        ThrowIfFailed(D3D12SerializeRootSignature(
            &rootSignatureDesc,
            D3D_ROOT_SIGNATURE_VERSION_1,
            &signature,
            &error));
        ThrowIfFailed(device->CreateRootSignature(
            0,
            signature->GetBufferPointer(),
            signature->GetBufferSize(),
            IID_PPV_ARGS(&rootSignature)));
    }

    DXPipeline::DXPipeline(
        const ComPtr<ID3D12Device>& device,
        PipelineResources& pipelineResources,
        VertexInputLayout& vertexInputLayout,
        ShaderModule& vertexShader,
        ShaderModule& fragmentShader) {
        auto& dxVertexInputLayout = static_cast<DXVertexInputLayout&>(vertexInputLayout);
        auto& dxPipelineResources = static_cast<DXPipelineResources&>(pipelineResources);
        auto& dxVertexShader = static_cast<DXShaderModule&>(vertexShader);
        auto& dxPixelShader = static_cast<DXShaderModule&>(fragmentShader);

        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.InputLayout = {
            dxVertexInputLayout.getInputElementDescs().data(),
            static_cast<UINT>(dxVertexInputLayout.getInputElementDescs().size())
        };
        psoDesc.pRootSignature = dxPipelineResources.getRootSignature().Get();
        psoDesc.VS = CD3DX12_SHADER_BYTECODE(dxVertexShader.getShader().Get());
        psoDesc.PS = CD3DX12_SHADER_BYTECODE(dxPixelShader.getShader().Get());
        psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        psoDesc.DepthStencilState.DepthEnable = FALSE;
        psoDesc.DepthStencilState.StencilEnable = FALSE;
        psoDesc.SampleMask = UINT_MAX;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        psoDesc.SampleDesc.Count = 1;
        ThrowAndPrintIfFailed(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState)), device.Get());
    }

    DXBuffer::DXBuffer(ComPtr<ID3D12Device> device, Type type, size_t size, size_t count) {
        auto heap = CD3DX12_HEAP_PROPERTIES(HeapType[type]);
        bufferSize = size * count;
        resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(static_cast<UINT64>(bufferSize));
        ThrowIfFailed(device->CreateCommittedResource(
            &heap,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            ResourceStates[type],
            nullptr,
            IID_PPV_ARGS(&buffer)));
        bufferView.BufferLocation = buffer->GetGPUVirtualAddress();
        bufferView.StrideInBytes = size;
        bufferView.SizeInBytes = bufferSize;
    }

    void DXBuffer::map() {
        CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
        ThrowIfFailed(buffer->Map(0, &readRange, &mappedAddress));
    }

    void DXBuffer::unmap() {
        buffer->Unmap(0, nullptr);
        mappedAddress = nullptr;
    }

    void DXBuffer::write(void* data, size_t size, size_t offset) {
        if (size == WHOLE_SIZE) {
            memcpy(mappedAddress, data, bufferSize);
        } else {
            memcpy(reinterpret_cast<unsigned char*>(mappedAddress) + offset, data, size);
        }
    }

}