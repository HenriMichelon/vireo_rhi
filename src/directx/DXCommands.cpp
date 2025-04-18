/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/backend/directx/Libraries.h"
module vireo.directx;

import vireo.tools;

import vireo.directx.descriptors;
import vireo.directx.pipelines;
import vireo.directx.resources;
import vireo.directx.tools;

namespace vireo {

    DXSubmitQueue::DXSubmitQueue(const ComPtr<ID3D12Device>& device, const CommandType type) :
        device{device} {
        const auto queueDesc = D3D12_COMMAND_QUEUE_DESC {
            .Type = DXCommandList::ListType[static_cast<int>(type)],
            .Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
        };
        dxCheck(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue)));
    }

    void DXSubmitQueue::submit(
        const shared_ptr<Fence>& fence,
        const shared_ptr<const SwapChain>&,
        const vector<shared_ptr<const CommandList>>& commandLists) const {
        submit(commandLists);
        const auto dxFence = static_pointer_cast<DXFence>(fence);
        dxCheck(commandQueue->Signal(dxFence->getFence().Get(), dxFence->getValue()));
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
        dxCheck(device->CreateFence(
            0,
            D3D12_FENCE_FLAG_NONE,
            IID_PPV_ARGS(&inFlightFence)));
        const HANDLE inFlightFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (inFlightFenceEvent == nullptr) {
            dxCheck(HRESULT_FROM_WIN32(GetLastError()));
        }
        dxCheck(commandQueue->Signal(inFlightFence.Get(), 0));
        if (inFlightFence->GetCompletedValue() < 0) {
            dxCheck(inFlightFence->SetEventOnCompletion(
                0,
                inFlightFenceEvent));
            WaitForSingleObjectEx(inFlightFenceEvent, INFINITE, FALSE);
        }
        CloseHandle(inFlightFenceEvent);
    }

    DXCommandAllocator::DXCommandAllocator( const ComPtr<ID3D12Device>& device, const CommandType type):
        CommandAllocator{type},
        device{device} {
        dxCheck(device->CreateCommandAllocator(
            DXCommandList::ListType[static_cast<int>(type)],
            IID_PPV_ARGS(&commandAllocator)));
    }

    void DXCommandAllocator::reset() const {
        dxCheck(commandAllocator->Reset());
    }

    shared_ptr<CommandList> DXCommandAllocator::createCommandList(const shared_ptr<const Pipeline>& pipeline) const {
        return make_shared<DXCommandList>(
            getCommandListType(),
            device,
            commandAllocator,
            static_pointer_cast<const DXGraphicPipeline>(pipeline)->getPipelineState());
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
        dxCheck(device->CreateCommandList(
            0,
            ListType[static_cast<int>(type)],
            commandAllocator.Get(),
            pipelineState == nullptr ? nullptr : pipelineState.Get(),
            IID_PPV_ARGS(&commandList)));
        dxCheck(commandList->Close());
    }

    void DXCommandList::bindPipeline(const shared_ptr<const Pipeline>& pipeline) {
        if (pipeline->getType() == Pipeline::COMPUTE) {
            commandList->SetPipelineState(static_pointer_cast<const DXComputePipeline>(pipeline)->getPipelineState().Get());
            commandList->SetComputeRootSignature(static_pointer_cast<const DXPipelineResources>(pipeline->getResources())->getRootSignature().Get());
        } else {
            const auto dxPipeline = static_pointer_cast<const DXGraphicPipeline>(pipeline);
            commandList->SetPipelineState(dxPipeline->getPipelineState().Get());
            commandList->SetGraphicsRootSignature(static_pointer_cast<const DXPipelineResources>(pipeline->getResources())->getRootSignature().Get());
            commandList->IASetPrimitiveTopology(dxPipeline->getPrimitiveTopology());
        }
    }

    void DXCommandList::bindDescriptors(
        const shared_ptr<const Pipeline>&pipeline,
        const vector<shared_ptr<const DescriptorSet>>& descriptors) const {
        if (descriptors.empty()) { return; }
        vector<ID3D12DescriptorHeap*> heaps(descriptors.size());
        for (int i = 0; i < descriptors.size(); i++) {
            heaps[i] = static_pointer_cast<const DXDescriptorSet>(descriptors[i])->getHeap().Get();
        }
        commandList->SetDescriptorHeaps(heaps.size(), heaps.data());
        if (pipeline->getType() == Pipeline::COMPUTE) {
            for (int i = 0; i < descriptors.size(); i++) {
                commandList->SetComputeRootDescriptorTable(i, heaps[i]->GetGPUDescriptorHandleForHeapStart());
            }
        } else {
            for (int i = 0; i < descriptors.size(); i++) {
                commandList->SetGraphicsRootDescriptorTable(i, heaps[i]->GetGPUDescriptorHandleForHeapStart());
            }
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

    void DXCommandList::beginRendering(
        const shared_ptr<SwapChain>& swapChain,
        const float clearColor[]) const {
        const auto dxSwapChain = static_pointer_cast<const DXSwapChain>(swapChain);
        const auto frameIndex = swapChain->getCurrentFrameIndex();
        const CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(
            dxSwapChain->getHeap()->GetCPUDescriptorHandleForHeapStart(),
            frameIndex,
            dxSwapChain->getDescriptorSize());
        beginRendering(
            rtvHandle,
            clearColor);
    }

    void DXCommandList::beginRendering(
        const shared_ptr<RenderTarget>& renderTarget,
        const float clearColor[]) const {
        const auto dxRenderTarget = static_pointer_cast<DXRenderTarget>(renderTarget);
        beginRendering(
            dxRenderTarget->getHandle(),
            clearColor
        );
    }

    void DXCommandList::beginRendering(
        const D3D12_CPU_DESCRIPTOR_HANDLE& handle,
        const float clearColor[]) const {
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

    void DXCommandList::dispatch(const uint32_t x, const uint32_t y, const uint32_t z) const {
        commandList->Dispatch(x, y, z);
    }

    void DXCommandList::barrier(
        const shared_ptr<const Image>& image,
        const ResourceState oldState,
        const ResourceState newState) const {
        const auto dxImage = static_pointer_cast<const DXImage>(image)->getImage();
        barrier(dxImage, oldState, newState);
    }

    void DXCommandList::barrier(
        const shared_ptr<const SwapChain>& swapChain,
        const ResourceState oldState,
        const ResourceState newState) const {
        const auto dxSwapChain = static_pointer_cast<const DXSwapChain>(swapChain);
        barrier(dxSwapChain->getRenderTargets()[dxSwapChain->getCurrentFrameIndex()], oldState, newState);
    }

    void DXCommandList::barrier(
       const ComPtr<ID3D12Resource>& resource,
       const ResourceState oldState,
       const ResourceState newState) const {
        D3D12_RESOURCE_STATES srcState, dstState;
        if (oldState == ResourceState::UNDEFINED && newState == ResourceState::DISPATCH_TARGET) {
            srcState = D3D12_RESOURCE_STATE_COMMON;
            dstState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
        } else if (oldState == ResourceState::UNDEFINED && newState == ResourceState::RENDER_TARGET) {
            srcState = D3D12_RESOURCE_STATE_COMMON;
            dstState = D3D12_RESOURCE_STATE_RENDER_TARGET;
        } else if (oldState == ResourceState::RENDER_TARGET && newState == ResourceState::PRESENT) {
            srcState = D3D12_RESOURCE_STATE_RENDER_TARGET;
            dstState = D3D12_RESOURCE_STATE_PRESENT;
        } else if (oldState == ResourceState::UNDEFINED && newState == ResourceState::PRESENT) {
            srcState = D3D12_RESOURCE_STATE_COMMON;
            dstState = D3D12_RESOURCE_STATE_PRESENT;
        } else if (oldState == ResourceState::COPY_DST && newState == ResourceState::PRESENT) {
            srcState = D3D12_RESOURCE_STATE_COPY_DEST;
            dstState = D3D12_RESOURCE_STATE_PRESENT;
        } else if (oldState == ResourceState::UNDEFINED && newState == ResourceState::COPY_DST) {
            srcState = D3D12_RESOURCE_STATE_COMMON;
            dstState = D3D12_RESOURCE_STATE_COPY_DEST;
        } else if (oldState == ResourceState::COPY_DST && newState == ResourceState::SHADER_READ) {
            srcState = D3D12_RESOURCE_STATE_COPY_DEST;
            dstState = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
        } else if (oldState == ResourceState::DISPATCH_TARGET && newState == ResourceState::COPY_SRC) {
            srcState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
            dstState = D3D12_RESOURCE_STATE_COPY_SOURCE;
        } else if (oldState == ResourceState::UNDEFINED && newState == ResourceState::COPY_SRC) {
            srcState = D3D12_RESOURCE_STATE_COMMON;
            dstState = D3D12_RESOURCE_STATE_COPY_SOURCE;
        } else if (oldState == ResourceState::COPY_SRC && newState == ResourceState::UNDEFINED) {
            srcState = D3D12_RESOURCE_STATE_COPY_SOURCE;
            dstState = D3D12_RESOURCE_STATE_COMMON;
        } else if (oldState == ResourceState::COPY_SRC && newState == ResourceState::DISPATCH_TARGET) {
            srcState = D3D12_RESOURCE_STATE_COPY_SOURCE;
            dstState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
        } else {
            throw Exception("Not implemented");
            return;
        }
        const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            resource.Get(),
            srcState,
            dstState);
        commandList->ResourceBarrier(1, &barrier);
    }

    void DXCommandList::pushConstants(
        const shared_ptr<const PipelineResources>& pipelineResources,
        const PushConstantsDesc& pushConstants,
        const void* data) const {
        const auto dxResources = static_pointer_cast<const DXPipelineResources>(pipelineResources);
        commandList->SetGraphicsRoot32BitConstants(
            dxResources->getPushConstantsRootParameterIndex(),
            pushConstants.size / sizeof(uint32_t),
            data,
            pushConstants.offset);
    }

    void DXCommandList::begin() const {
        dxCheck(commandList->Reset(commandAllocator.Get(), nullptr));
    }

    void DXCommandList::end() const {
        dxCheck(commandList->Close());
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
            dxCheck(device->CreateCommittedResource(
                &stagingHeapProps,
                D3D12_HEAP_FLAG_NONE,
                &stagingResourceDesc,
                D3D12_RESOURCE_STATE_COMMON,
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
            dxCheck(device->CreateCommittedResource(
                &stagingHeapProps,
                D3D12_HEAP_FLAG_NONE,
                &stagingResourceDesc,
                D3D12_RESOURCE_STATE_COMMON,
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
        stagingBuffers.push_back(stagingBuffer);
    }

    void DXCommandList::copy(
        const shared_ptr<const Image>& source,
        const shared_ptr<const SwapChain>& swapChain) const {
        const auto dxSource = static_pointer_cast<const DXImage>(source);
        const auto dxSwapChain = static_pointer_cast<const DXSwapChain>(swapChain);

        const auto srcLocation = D3D12_TEXTURE_COPY_LOCATION {
            .pResource = dxSource->getImage().Get(),
            .Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
            .SubresourceIndex = 0,
        };
        const auto dstLocation = D3D12_TEXTURE_COPY_LOCATION {
            .pResource = dxSwapChain->getRenderTargets()[dxSwapChain->getCurrentFrameIndex()].Get(),
            .Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
            .SubresourceIndex = 0,
        };

        commandList->CopyTextureRegion(&dstLocation, 0, 0, 0, &srcLocation, nullptr);
    }

    void DXCommandList::blit(
       const shared_ptr<const Image>& source,
       const shared_ptr<const SwapChain>& swapChain,
       Filter filter) const {
       copy(source, swapChain);
    }

}
