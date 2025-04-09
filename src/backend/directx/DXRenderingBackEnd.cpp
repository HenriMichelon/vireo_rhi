/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "backend/directx/Tools.h"

module vireo.backend.directx;

import vireo.app.win32;
import vireo.backend.directx.buffer;
import vireo.backend.directx.framedata;

namespace vireo::backend {

    DXRenderingBackEnd::DXRenderingBackEnd() {
        // Detect RivaTuner which cause problem by incorrectly hooking IDXGISwapChain::Present
        HANDLE hMap = OpenFileMapping(FILE_MAP_READ, FALSE, L"RTSSSharedMemoryV2");
        if (hMap) {
            MessageBox(
                nullptr,
                L"RivaTuner Statistic Server is incompatible with the DirectX 12 backend, close it or use the Vulkan backend",
                nullptr,
                MB_OK);
            CloseHandle(hMap);
            die("RTSS conflict");
        }

        RECT windowRect{};
        if (GetClientRect(Win32Application::getHwnd(), &windowRect) == 0) {
            die("Error getting window rect");
        }
        auto width = windowRect.right - windowRect.left;
        auto height = windowRect.bottom - windowRect.top;
        viewport = CD3DX12_VIEWPORT{0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height)},
        scissorRect = CD3DX12_RECT{0, 0, static_cast<LONG>(width), static_cast<LONG>(height)};

        instance = std::make_shared<DXInstance>();
        physicalDevice = std::make_shared<DXPhysicalDevice>(getDXInstance()->getFactory());
        device = std::make_shared<DXDevice>(getDXPhysicalDevice()->getHardwareAdapter());
        graphicCommandQueue = std::make_shared<DXSubmitQueue>(CommandList::GRAPHIC, getDXDevice()->getDevice());
        transferCommandQueue = std::make_shared<DXSubmitQueue>(CommandList::GRAPHIC, getDXDevice()->getDevice());
        swapChain = std::make_shared<DXSwapChain>(
            getDXInstance()->getFactory(),
            *getDXDevice(),
            getDXGraphicCommandQueue()->getCommandQueue(),
            width,
            height);
    }

    std::shared_ptr<FrameData> DXRenderingBackEnd::createFrameData(const uint32_t frameIndex) {
        return std::make_shared<DXFrameData>();
    }

    std::shared_ptr<PipelineResources> DXRenderingBackEnd::createPipelineResources() const {
        return std::make_shared<DXPipelineResources>(getDXDevice()->getDevice());
    }

    std::shared_ptr<Pipeline> DXRenderingBackEnd::createPipeline(
        PipelineResources& pipelineResources,
        VertexInputLayout& vertexInputLayout,
        ShaderModule& vertexShader,
        ShaderModule& fragmentShader,
        const std::wstring& name) const {
        return std::make_shared<DXPipeline>(
            getDXDevice()->getDevice(),
            pipelineResources,
            vertexInputLayout,
            vertexShader,
            fragmentShader,
            name);
    }

    std::shared_ptr<VertexInputLayout> DXRenderingBackEnd::createVertexLayout(
        size_t,
        const std::vector<VertexInputLayout::AttributeDescription>& attributesDescriptions) const {
        return std::make_shared<DXVertexInputLayout>(attributesDescriptions);
    }

    std::shared_ptr<ShaderModule> DXRenderingBackEnd::createShaderModule(const std::string& fileName) const {
        return std::make_shared<DXShaderModule>(fileName);
    }

    std::shared_ptr<Buffer> DXRenderingBackEnd::createBuffer(const Buffer::Type type,
                                                             const size_t size,
                                                             const size_t count,
                                                             const size_t alignment,
                                                             const std::wstring& name) const {
        return make_shared<DXBuffer>(getDXDevice()->getDevice(), type, size, count, alignment, name);
    }

    void DXRenderingBackEnd::beginRendering(FrameData&,
                                            PipelineResources& pipelineResources,
                                            Pipeline& pipeline,
                                            CommandList& commandList) {
        const auto dxCommandList = static_cast<DXCommandList&>(commandList).getCommandList();
        const auto dxSwapChain = getDXSwapChain();
        const auto frameIndex = swapChain->getCurrentFrameIndex();
        const auto& dxPipelineResources = static_cast<DXPipelineResources&>(pipelineResources);
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


        const float dxClearColor[] = {clearColor.r, clearColor.g, clearColor.b, clearColor.a};
        dxCommandList->ClearRenderTargetView(
            rtvHandle,
            dxClearColor,
            0,
            nullptr);
        dxCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    }

    void DXRenderingBackEnd::endRendering(CommandList& commandList) {
        const auto dxCommandList = static_cast<DXCommandList&>(commandList).getCommandList();
        const auto dxSwapChain = getDXSwapChain();
        const auto frameIndex = swapChain->getCurrentFrameIndex();

        const auto swapChainBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
            dxSwapChain->getRenderTargets()[frameIndex].Get(),
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            D3D12_RESOURCE_STATE_PRESENT);
        dxCommandList->ResourceBarrier(1, &swapChainBarrier);
    }

    std::shared_ptr<CommandAllocator> DXRenderingBackEnd::createCommandAllocator(CommandList::Type type) const {
        return std::make_shared<DXCommandAllocator>(type, getDXDevice()->getDevice());
    }

    void DXRenderingBackEnd::waitIdle() {
        graphicCommandQueue->waitIdle();
    }

    DXSubmitQueue::DXSubmitQueue(const CommandList::Type type, const ComPtr<ID3D12Device>& device) :
        device{device} {
        const auto queueDesc = D3D12_COMMAND_QUEUE_DESC{
            .Type = DXCommandList::ListType[type],
            .Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
        };
        DieIfFailed(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue)));
    }

    void DXSubmitQueue::submit(const FrameData& frameData, std::vector<std::shared_ptr<CommandList>> commandLists) {
        submit(commandLists);
    }

    void DXSubmitQueue::submit(const std::vector<std::shared_ptr<CommandList>> commandLists) {
        auto dxCommandLists = std::vector<ID3D12CommandList*>(commandLists.size());
        for (int i = 0; i < commandLists.size(); i++) {
            dxCommandLists[i] = static_pointer_cast<DXCommandList>(commandLists[i])->getCommandList().Get();
        }
        commandQueue->ExecuteCommandLists(dxCommandLists.size(), dxCommandLists.data());
    }

    void DXSubmitQueue::waitIdle() {
        ComPtr<ID3D12Fence> inFlightFence;
        DieIfFailed(device->CreateFence(
            0,
            D3D12_FENCE_FLAG_NONE,
            IID_PPV_ARGS(&inFlightFence)));
        HANDLE inFlightFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (inFlightFenceEvent == nullptr) {
            DieIfFailed(HRESULT_FROM_WIN32(GetLastError()));
        }
        DieIfFailed(commandQueue->Signal(inFlightFence.Get(), 0));
        if (inFlightFence->GetCompletedValue() < 0) {
            DieIfFailed(inFlightFence->SetEventOnCompletion(
                0,
                inFlightFenceEvent
                ));
            WaitForSingleObjectEx(inFlightFenceEvent, INFINITE, FALSE);
        }
        CloseHandle(inFlightFenceEvent);
    }

    DXCommandAllocator::DXCommandAllocator(const CommandList::Type type, const ComPtr<ID3D12Device>& device):
        CommandAllocator{type},
        device{device} {
        DieIfFailed(device->CreateCommandAllocator(
            DXCommandList::ListType[type],
            IID_PPV_ARGS(&commandAllocator)));
    }

    std::shared_ptr<CommandList> DXCommandAllocator::createCommandList(Pipeline& pipeline) const {
        return std::make_shared<DXCommandList>(
            getCommandListType(),
            device,
            commandAllocator,
            static_cast<DXPipeline&>(pipeline).getPipelineState());
    }

    std::shared_ptr<CommandList> DXCommandAllocator::createCommandList() const {
        return std::make_shared<DXCommandList>(
            getCommandListType(),
            device,
            commandAllocator,
            nullptr);
    }

    DXCommandList::DXCommandList(
        const Type type,
        const ComPtr<ID3D12Device>& device,
        const ComPtr<ID3D12CommandAllocator>& commandAllocator,
        const ComPtr<ID3D12PipelineState>& pipelineState):
        device{device},
        commandAllocator{commandAllocator} {
        DieIfFailed(device->CreateCommandList(
            0,
            ListType[type],
            commandAllocator.Get(),
            pipelineState == nullptr ? nullptr : pipelineState.Get(),
            IID_PPV_ARGS(&commandList)));
        DieIfFailed(commandList->Close());
    }

    void DXCommandList::reset() {
        DieIfFailed(commandAllocator->Reset());
    }

    void DXCommandList::begin(Pipeline& pipeline) {
        auto& dxPipeline = static_cast<DXPipeline&>(pipeline);
        DieIfFailed(commandList->Reset(commandAllocator.Get(), dxPipeline.getPipelineState().Get()));
    }

    void DXCommandList::begin() {
        DieIfFailed(commandList->Reset(commandAllocator.Get(), nullptr));
    }

    void DXCommandList::end() {
        DieIfFailed(commandList->Close());
    }

    void DXCommandList::cleanup() {
        stagingBuffers.clear();
    }

    void DXCommandList::bindVertexBuffer(Buffer& buffer) {
        auto& vertexBuffer = static_cast<DXBuffer&>(buffer);
        commandList->IASetVertexBuffers(0, 1, &vertexBuffer.getBufferView());
    }

    void DXCommandList::drawInstanced(const uint32_t vertexCountPerInstance, const uint32_t instanceCount) {
        commandList->DrawInstanced(vertexCountPerInstance, instanceCount, 0, 0);
    }

    void DXCommandList::upload(Buffer& destination, const void* source) {
        auto& buffer = static_cast<DXBuffer&>(destination);

        ComPtr<ID3D12Resource> stagingBuffer;
        const auto stagingBufferSize = GetRequiredIntermediateSize(buffer.getBuffer().Get(), 0, 1);
        const auto stagingHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        const auto stagingResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(stagingBufferSize);
        DieIfFailed(device->CreateCommittedResource(
            &stagingHeapProps,
            D3D12_HEAP_FLAG_NONE,
            &stagingResourceDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&stagingBuffer)));

        const auto copyData = D3D12_SUBRESOURCE_DATA{
            .pData = source,
            .RowPitch = static_cast<LONG_PTR>(buffer.getSize()),
            .SlicePitch = static_cast<LONG_PTR>(buffer.getSize()),
        };
        UpdateSubresources(
            commandList.Get(),
            buffer.getBuffer().Get(),
            stagingBuffer.Get(),
            0,
            0,
            1,
            &copyData);
        const auto memoryBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
            buffer.getBuffer().Get(),
            D3D12_RESOURCE_STATE_COPY_DEST,
            DXBuffer::ResourceStates[buffer.getType()]);
        commandList->ResourceBarrier(1, &memoryBarrier);

        stagingBuffers.push_back(stagingBuffer);
    }


    DXSwapChain::DXSwapChain(
        const ComPtr<IDXGIFactory4>& factory,
        DXDevice& dxdevice,
        const ComPtr<ID3D12CommandQueue>& commandQueue,
        const uint32_t width,
        const uint32_t height) :
        device{dxdevice},
        presentCommandQueue{commandQueue} {
        extent = {width, height};
        aspectRatio = static_cast<float>(width) / static_cast<float>(height);

        const auto swapChainDesc = DXGI_SWAP_CHAIN_DESC1{
            .Width = width,
            .Height = height,
            .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
            .SampleDesc = {.Count = 1},
            .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
            .BufferCount = FRAMES_IN_FLIGHT,
            .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
        };

        ComPtr<IDXGISwapChain1> swapChain1;
        DieIfFailed(factory->CreateSwapChainForHwnd(
            commandQueue.Get(),
            // Swap chain needs the queue so that it can force a flush on it.
            Win32Application::getHwnd(),
            &swapChainDesc,
            nullptr,
            nullptr,
            &swapChain1
            ));
        DieIfFailed(swapChain1.As(&swapChain));
        currentFrameIndex = swapChain->GetCurrentBackBufferIndex();

        // Describe and create a render target view (RTV) descriptor heap.
        constexpr auto rtvHeapDesc = D3D12_DESCRIPTOR_HEAP_DESC{
            .Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
            .NumDescriptors = FRAMES_IN_FLIGHT,
            .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
        };
        DieIfFailed(device.getDevice()->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap)));
        rtvDescriptorSize = device.getDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        // Create frame resources.
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());
        constexpr auto rtvDesc = D3D12_RENDER_TARGET_VIEW_DESC{
            .Format = RENDER_FORMAT,
            .ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D,
        };
        for (UINT n = 0; n < FRAMES_IN_FLIGHT; n++) {
            DieIfFailed(swapChain->GetBuffer(n, IID_PPV_ARGS(&renderTargets[n])));
            renderTargets[n]->SetName(L"BackBuffer");
            device.getDevice()->CreateRenderTargetView(renderTargets[n].Get(), &rtvDesc, rtvHandle);
            rtvHandle.Offset(1, rtvDescriptorSize);
        }
    }

    void DXSwapChain::nextSwapChain() {
        currentFrameIndex = swapChain->GetCurrentBackBufferIndex();
        assert(currentFrameIndex < FRAMES_IN_FLIGHT);
    }

    bool DXSwapChain::acquire(FrameData& frameData) {
        auto& data = static_cast<DXFrameData&>(frameData);
        const auto currentFenceValue = data.inFlightFenceValue;
        DieIfFailed(presentCommandQueue->Signal(device.getInFlightFence().Get(), currentFenceValue));
        // If the next frame is not ready to be rendered yet, wait until it is ready.
        if (device.getInFlightFence()->GetCompletedValue() < currentFenceValue) {
            DieIfFailed(device.getInFlightFence()->SetEventOnCompletion(
                currentFenceValue,
                device.getInFlightFenceEvent()
                ));
            WaitForSingleObjectEx(device.getInFlightFenceEvent(), INFINITE, FALSE);
        }
        return true;
    }

    void DXSwapChain::present(FrameData& frameData) {
        auto& data = static_cast<DXFrameData&>(frameData);
        DieIfFailed(swapChain->Present(1, 0));
        data.inFlightFenceValue += 1;
    }

    DXVertexInputLayout::DXVertexInputLayout(const std::vector<AttributeDescription>& attributesDescriptions) {
        for (int i = 0; i < attributesDescriptions.size(); i++) {
            inputElementDescs.push_back({
                .SemanticName = attributesDescriptions[i].binding.c_str(),
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
        DieIfFailed(D3D12SerializeRootSignature(
            &rootSignatureDesc,
            D3D_ROOT_SIGNATURE_VERSION_1,
            &signature,
            &error));
        DieIfFailed(device->CreateRootSignature(
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
        ShaderModule& fragmentShader,
        const std::wstring& name) {
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
        psoDesc.RTVFormats[0] = DXSwapChain::RENDER_FORMAT;
        psoDesc.SampleDesc.Count = 1;
        DieIfFailed(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState)));
        pipelineState->SetName(name.c_str());
    }


}
