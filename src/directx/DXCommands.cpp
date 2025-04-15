/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/backend/directx/Tools.h"
module vireo.directx;

import vireo.directx.descriptors;
import vireo.directx.pipelines;
import vireo.directx.resources;

namespace vireo {

    DXSubmitQueue::DXSubmitQueue(const ComPtr<ID3D12Device>& device, const CommandType type) :
        device{device} {
        const auto queueDesc = D3D12_COMMAND_QUEUE_DESC {
            .Type = DXCommandList::ListType[static_cast<int>(type)],
            .Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
        };
        DieIfFailed(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue)));
    }

    void DXSubmitQueue::submit(
        const shared_ptr<const FrameData>&,
        const vector<shared_ptr<const CommandList>>& commandLists) const {
        submit(commandLists);
    }

    void DXSubmitQueue::submit(const vector<shared_ptr<const CommandList>>& commandLists) const {
        auto dxCommandLists = vector<ID3D12CommandList*>(commandLists.size());
        for (int i = 0; i < commandLists.size(); i++) {
            dxCommandLists[i] = static_pointer_cast<const DXCommandList>(commandLists[i])->getCommandList().Get();
        }
        commandQueue->ExecuteCommandLists(dxCommandLists.size(), dxCommandLists.data());
    }

    void DXSubmitQueue::waitIdle() const {
        ComPtr<ID3D12Fence> inFlightFence;
        DieIfFailed(device->CreateFence(
            0,
            D3D12_FENCE_FLAG_NONE,
            IID_PPV_ARGS(&inFlightFence)));
        const HANDLE inFlightFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (inFlightFenceEvent == nullptr) {
            DieIfFailed(HRESULT_FROM_WIN32(GetLastError()));
        }
        DieIfFailed(commandQueue->Signal(inFlightFence.Get(), 0));
        if (inFlightFence->GetCompletedValue() < 0) {
            DieIfFailed(inFlightFence->SetEventOnCompletion(
                0,
                inFlightFenceEvent));
            WaitForSingleObjectEx(inFlightFenceEvent, INFINITE, FALSE);
        }
        CloseHandle(inFlightFenceEvent);
    }

    DXCommandAllocator::DXCommandAllocator( const ComPtr<ID3D12Device>& device, const CommandType type):
        CommandAllocator{type},
        device{device} {
        DieIfFailed(device->CreateCommandAllocator(
            DXCommandList::ListType[static_cast<int>(type)],
            IID_PPV_ARGS(&commandAllocator)));
    }

    void DXCommandAllocator::reset() const {
        DieIfFailed(commandAllocator->Reset());
    }

    shared_ptr<CommandList> DXCommandAllocator::createCommandList(const shared_ptr<const Pipeline>& pipeline) const {
        return make_shared<DXCommandList>(
            getCommandListType(),
            device,
            commandAllocator,
            static_pointer_cast<const DXPipeline>(pipeline)->getPipelineState());
    }

    shared_ptr<CommandList> DXCommandAllocator::createCommandList() const {
        return make_shared<DXCommandList>(
            getCommandListType(),
            device,
            commandAllocator,
            nullptr);
    }

    DXCommandList::DXCommandList(
        const CommandType type,
        const ComPtr<ID3D12Device>& device,
        const ComPtr<ID3D12CommandAllocator>& commandAllocator,
        const ComPtr<ID3D12PipelineState>& pipelineState):
        device{device},
        commandAllocator{commandAllocator} {
        DieIfFailed(device->CreateCommandList(
            0,
            ListType[static_cast<int>(type)],
            commandAllocator.Get(),
            pipelineState == nullptr ? nullptr : pipelineState.Get(),
            IID_PPV_ARGS(&commandList)));
        DieIfFailed(commandList->Close());
    }

    void DXCommandList::bindPipeline(const shared_ptr<const Pipeline>& pipeline) {
        commandList->SetPipelineState(static_pointer_cast<const DXPipeline>(pipeline)->getPipelineState().Get());
        commandList->SetGraphicsRootSignature(static_pointer_cast<const DXPipelineResources>(pipeline->getResources())->getRootSignature().Get());
    }

    void DXCommandList::bindDescriptors(const vector<shared_ptr<const DescriptorSet>>& descriptors) const {
        if (descriptors.empty()) { return; }
        vector<ID3D12DescriptorHeap*> heaps(descriptors.size());
        for (int i = 0; i < descriptors.size(); i++) {
            heaps[i] = static_pointer_cast<const DXDescriptorSet>(descriptors[i])->getHeap().Get();
        }
        commandList->SetDescriptorHeaps(heaps.size(), heaps.data());
        for (int i = 0; i < descriptors.size(); i++) {
            commandList->SetGraphicsRootDescriptorTable(i, heaps[i]->GetGPUDescriptorHandleForHeapStart());
        }
    }

    void DXCommandList::setViewports(const uint32_t count, const vector<Extent>& extent) const {
        vector<CD3DX12_VIEWPORT> viewports(count);
        for (int i = 0; i < count; i++) {
            viewports[i].TopLeftX = 0.0f;
            viewports[i].TopLeftY = 0.0f;
            viewports[i].Width = static_cast<FLOAT>(extent[i].width);
            viewports[i].Height = static_cast<FLOAT>(extent[i].height);
            viewports[i].MinDepth = 0.0f;
            viewports[i].MaxDepth = 0.0f;
        }
        commandList->RSSetViewports(count, viewports.data());
    }

    void DXCommandList::setScissors(const uint32_t count, const vector<Extent>& extent) const {
        vector<CD3DX12_RECT> scissors(count);
        for (int i = 0; i < count; i++) {
            scissors[i].left = 0;
            scissors[i].top = 0;
            scissors[i].right = extent[i].width;
            scissors[i].bottom = extent[i].height;
        }
        commandList->RSSetScissorRects(1, scissors.data());
    }

    void DXCommandList::setPrimitiveTopology(PrimitiveTopology primitiveTopology) const {
        commandList->IASetPrimitiveTopology(dxPrimitives[static_cast<int>(primitiveTopology)]);
    }

    void DXCommandList::beginRendering(
        const shared_ptr<FrameData>& frameData,
        const shared_ptr<SwapChain>& swapChain,
        const float clearColor[]) const {
        const auto dxSwapChain = static_pointer_cast<const DXSwapChain>(swapChain);
        const auto frameIndex = swapChain->getCurrentFrameIndex();
        const CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(
            dxSwapChain->getHeap()->GetCPUDescriptorHandleForHeapStart(),
            frameIndex,
            dxSwapChain->getDescriptorSize());
        beginRendering(
            dxSwapChain->getRenderTargets()[frameIndex],
            rtvHandle,
            clearColor);
    }

    void DXCommandList::beginRendering(
        const shared_ptr<RenderTarget>& renderTarget,
        const float clearColor[]) const {
        const auto dxRenderTarget = static_pointer_cast<DXRenderTarget>(renderTarget);
        beginRendering(
            static_pointer_cast<DXImage>(dxRenderTarget->getImage())->getImage(),
            dxRenderTarget->getHandle(),
            clearColor
        );
    }

    void DXCommandList::beginRendering(
        const ComPtr<ID3D12Resource>& resource,
        const D3D12_CPU_DESCRIPTOR_HANDLE& handle,
        const float clearColor[]) const {

        const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            resource.Get(),
            D3D12_RESOURCE_STATE_COMMON,
            D3D12_RESOURCE_STATE_RENDER_TARGET);
        commandList->ResourceBarrier(1, &barrier);

        commandList->OMSetRenderTargets(
            1,
            &handle,
            FALSE,
            nullptr);

        const float dxClearColor[] = {clearColor[0], clearColor[1], clearColor[2], clearColor[3]};
        commandList->ClearRenderTargetView(
            handle,
            dxClearColor,
            0,
            nullptr);
    }

    void DXCommandList::endRendering(const shared_ptr<const FrameData>& frameData, const shared_ptr<SwapChain>& swapChain) const {
        const auto dxSwapChain = static_pointer_cast<const DXSwapChain>(swapChain);
        const auto swapChainBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
            dxSwapChain->getRenderTargets()[dxSwapChain->getCurrentFrameIndex()].Get(),
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            D3D12_RESOURCE_STATE_PRESENT);
        commandList->ResourceBarrier(1, &swapChainBarrier);
    }

    void DXCommandList::endRendering(const shared_ptr<RenderTarget>& renderTarget) const {
        const auto dxImage = static_pointer_cast<const DXImage>(renderTarget->getImage());
        const auto swapChainBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
            dxImage->getImage().Get(),
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            D3D12_RESOURCE_STATE_COMMON);
        commandList->ResourceBarrier(1, &swapChainBarrier);
    }

    void DXCommandList::begin() const {
        DieIfFailed(commandList->Reset(commandAllocator.Get(), nullptr));
    }

    void DXCommandList::end() const {
        DieIfFailed(commandList->Close());
    }

    void DXCommandList::cleanup() {
        for (int i = 0; i < stagingBuffers.size(); i++) {
            // stagingBuffers[i]->Release();
        }
        stagingBuffers.clear();
    }

    void DXCommandList::bindVertexBuffer(const shared_ptr<const Buffer>& buffer) const {
        const auto& vertexBuffer = static_pointer_cast<const DXBuffer>(buffer);
        commandList->IASetVertexBuffers(0, 1, &vertexBuffer->getBufferView());
    }

    void DXCommandList::drawInstanced(const uint32_t vertexCountPerInstance, const uint32_t instanceCount) const {
        commandList->DrawInstanced(vertexCountPerInstance, instanceCount, 0, 0);
    }

    void DXCommandList::upload(const shared_ptr<const Buffer>& destination, const void* source) {
        const auto buffer = static_pointer_cast<const DXBuffer>(destination);

        ComPtr<ID3D12Resource> stagingBuffer;
        {
            const auto stagingBufferSize = GetRequiredIntermediateSize(buffer->getBuffer().Get(), 0, 1);
            const auto stagingHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
            const auto stagingResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(stagingBufferSize);
            DieIfFailed(device->CreateCommittedResource(
                &stagingHeapProps,
                D3D12_HEAP_FLAG_NONE,
                &stagingResourceDesc,
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&stagingBuffer)));
#ifdef _DEBUG
            stagingBuffer->SetName(L"stagingBuffer buffer");
#endif
        }

        {
            const auto copyData = D3D12_SUBRESOURCE_DATA {
                .pData = source,
                .RowPitch = static_cast<LONG_PTR>(buffer->getSize()),
                .SlicePitch = static_cast<LONG_PTR>(buffer->getSize()),
            };
            UpdateSubresources(
                commandList.Get(),
                buffer->getBuffer().Get(),
                stagingBuffer.Get(),
                0,
                0,
                1,
                &copyData);
        }

        {
            const auto memoryBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
                buffer->getBuffer().Get(),
                D3D12_RESOURCE_STATE_COPY_DEST,
                DXBuffer::ResourceStates[static_cast<int>(buffer->getType())]);
            commandList->ResourceBarrier(1, &memoryBarrier);
        }

        stagingBuffers.push_back(stagingBuffer);
    }

    void DXCommandList::upload(const shared_ptr<const Image>& destination, const void* source) {
        const auto image = static_pointer_cast<const DXImage>(destination);

        auto stagingBuffer = ComPtr<ID3D12Resource>{nullptr};
        {
            const auto stagingBufferSize = GetRequiredIntermediateSize(image->getImage().Get(), 0, 1);
            const auto stagingHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
            const auto stagingResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(stagingBufferSize);
            DieIfFailed(device->CreateCommittedResource(
                &stagingHeapProps,
                D3D12_HEAP_FLAG_NONE,
                &stagingResourceDesc,
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&stagingBuffer)));
#ifdef _DEBUG
            stagingBuffer->SetName(L"stagingBuffer image");
#endif
        }

        {
            const auto copyData = D3D12_SUBRESOURCE_DATA {
                .pData = source,
                .RowPitch = static_cast<LONG_PTR>(image->getRowPitch()),
                .SlicePitch = static_cast<LONG_PTR>(image->getSize()),
            };
            UpdateSubresources(
                commandList.Get(),
                image->getImage().Get(),
                stagingBuffer.Get(),
                0,
                0,
                1,
                &copyData);
        }

        {
            const auto memoryBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
                image->getImage().Get(),
                D3D12_RESOURCE_STATE_COPY_DEST,
                D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE |
                    D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE); // enforced by DATA_STATIC in the root signature
            commandList->ResourceBarrier(1, &memoryBarrier);
        }

        stagingBuffers.push_back(stagingBuffer);
    }
}
