/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/backend/directx/Libraries.h"
module vireo.directx.commands;

import std;
import vireo.tools;
import vireo.directx.descriptors;
import vireo.directx.devices;
import vireo.directx.pipelines;
import vireo.directx.resources;
import vireo.directx.swapchains;
import vireo.directx.tools;

namespace vireo {

    DXSubmitQueue::DXSubmitQueue(const ComPtr<ID3D12Device>& device, const CommandType type) :
        device{device} {
        const auto queueDesc = D3D12_COMMAND_QUEUE_DESC {
            .Type = DXCommandList::dxType[static_cast<int>(type)],
            .Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
        };
        dxCheck(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue)));
    }

    void DXSubmitQueue::submit(
        const std::shared_ptr<Fence>& fence,
        const std::shared_ptr<const SwapChain>&,
        const std::vector<std::shared_ptr<const CommandList>>& commandLists) const {
        submit(fence, commandLists);
    }

    void DXSubmitQueue::submit(
        const std::shared_ptr<Fence>& fence,
        const std::vector<std::shared_ptr<const CommandList>>& commandLists) const {
        assert(fence);
        submit(commandLists);
        const auto dxFence = static_pointer_cast<DXFence>(fence);
        dxCheck(commandQueue->Signal(dxFence->getFence().Get(), dxFence->getValue()));
    }

    void DXSubmitQueue::submit(const std::vector<std::shared_ptr<const CommandList>>& commandLists) const {
        assert(commandLists.size() > 0);
        auto dxCommandLists = std::vector<ID3D12CommandList*>(commandLists.size());
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
        dxCheck(commandQueue->Signal(inFlightFence.Get(), 1));
        if (inFlightFence->GetCompletedValue() < 1) {
            dxCheck(inFlightFence->SetEventOnCompletion(
                1,
                inFlightFenceEvent));
            WaitForSingleObjectEx(inFlightFenceEvent, INFINITE, FALSE);
        }
        CloseHandle(inFlightFenceEvent);
    }

    DXCommandAllocator::DXCommandAllocator( const ComPtr<ID3D12Device>& device, const CommandType type):
        CommandAllocator{type},
        device{device} {
        dxCheck(device->CreateCommandAllocator(
            DXCommandList::dxType[static_cast<int>(type)],
            IID_PPV_ARGS(&commandAllocator)));
    }

    void DXCommandAllocator::reset() const {
        dxCheck(commandAllocator->Reset());
    }

    std::shared_ptr<CommandList> DXCommandAllocator::createCommandList(const std::shared_ptr<const Pipeline>& pipeline) const {
        assert(pipeline != nullptr);
        return std::make_shared<DXCommandList>(
            getCommandListType(),
            device,
            commandAllocator,
            static_pointer_cast<const DXGraphicPipeline>(pipeline)->getPipelineState());
    }

    std::shared_ptr<CommandList> DXCommandAllocator::createCommandList() const {
        return std::make_shared<DXCommandList>(
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
            dxType[static_cast<int>(type)],
            commandAllocator.Get(),
            pipelineState == nullptr ? nullptr : pipelineState.Get(),
            IID_PPV_ARGS(&commandList)));
        dxCheck(commandList->Close());
    }

    DXCommandList::~DXCommandList() {
        cleanup();
    }

    void DXCommandList::bindPipeline(const std::shared_ptr<const Pipeline>& pipeline) {
        assert(pipeline != nullptr);
        if (pipeline->getType() == PipelineType::COMPUTE) {
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
        const std::shared_ptr<const Pipeline>&pipeline,
        const std::vector<std::shared_ptr<const DescriptorSet>>& descriptors) const {
        assert(pipeline != nullptr);
        assert(descriptors.size() > 0);
        if (descriptors.empty()) { return; }
        std::vector<ID3D12DescriptorHeap*> heaps(descriptors.size());
        for (int i = 0; i < descriptors.size(); i++) {
            heaps[i] = static_pointer_cast<const DXDescriptorSet>(descriptors[i])->getHeap().Get();
        }
        commandList->SetDescriptorHeaps(heaps.size(), heaps.data());
        D3D12_GPU_DESCRIPTOR_HANDLE handle;
        for (int i = 0; i < descriptors.size(); i++) {
#ifdef _MSC_VER
            handle = heaps[i]->GetGPUDescriptorHandleForHeapStart();
#else
            heaps[i]->GetGPUDescriptorHandleForHeapStart(&handle);
#endif
            if (pipeline->getType() == PipelineType::COMPUTE) {
                commandList->SetComputeRootDescriptorTable(i, handle);
            } else {
                commandList->SetGraphicsRootDescriptorTable(i, handle);
            }
        }
    }

    void DXCommandList::setViewports(const std::vector<Extent>& extents) const {
        std::vector<CD3DX12_VIEWPORT> viewports(extents.size());
        for (int i = 0; i < viewports.size(); i++) {
            viewports[i].TopLeftX = 0.0f;
            viewports[i].TopLeftY = 0.0f;
            viewports[i].Width = static_cast<FLOAT>(extents[i].width);
            viewports[i].Height = static_cast<FLOAT>(extents[i].height);
            viewports[i].MinDepth = 0.0f;
            viewports[i].MaxDepth = 1.0f;
        }
        commandList->RSSetViewports(viewports.size(), viewports.data());
    }

    void DXCommandList::setScissors(const std::vector<Extent>& extents) const {
        std::vector<CD3DX12_RECT> scissors(extents.size());
        for (int i = 0; i < scissors.size(); i++) {
            scissors[i].left = 0;
            scissors[i].top = 0;
            scissors[i].right = extents[i].width;
            scissors[i].bottom = extents[i].height;
        }
        commandList->RSSetScissorRects(scissors.size(), scissors.data());
    }


    void DXCommandList::setViewport(const Extent& extent) const {
        const auto viewport = D3D12_VIEWPORT{
            .TopLeftX = 0.0f,
            .TopLeftY = 0.0f,
            .Width = static_cast<FLOAT>(extent.width),
            .Height = static_cast<FLOAT>(extent.height),
            .MinDepth = 0.0f,
            .MaxDepth = 1.0f,
        };
        commandList->RSSetViewports(1, &viewport);
    }

    void DXCommandList::setScissors(const Extent& extent) const {
        const auto scissor = D3D12_RECT{
            .left = 0,
            .top = 0,
            .right = static_cast<LONG>(extent.width),
            .bottom = static_cast<LONG>(extent.height),
        };
        commandList->RSSetScissorRects(1, &scissor);
    }

    void DXCommandList::beginRendering(const RenderingConfiguration& conf) {
        const auto dxDepthImage =
            conf.depthRenderTarget ? static_pointer_cast<DXRenderTarget>(conf.depthRenderTarget) : nullptr;
        D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle{};
        if (conf.multisampledDepthRenderTarget) {
            const auto dxMsaaTarget = static_pointer_cast<DXRenderTarget>(conf.multisampledDepthRenderTarget);
            dsvHandle = dxMsaaTarget->getHandle();
        } else if (dxDepthImage) {
            dsvHandle = dxDepthImage->getHandle();
        }
        const bool haveDepthResource = dxDepthImage || conf.multisampledDepthRenderTarget;
        if (conf.clearDepth && haveDepthResource) {
            commandList->ClearDepthStencilView(
                dsvHandle,
                D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
                conf.depthClearValue.depthStencil.depth, conf.depthClearValue.depthStencil.stencil,
                0, nullptr);
            if (conf.discardDepthAfterRender) {
                depthTargetToDiscard = static_pointer_cast<DXImage>(dxDepthImage->getImage())->getImage();
            }
        }

        auto rtvHandles = std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>(conf.colorRenderTargets.size()) ;
        for (int i = 0; i < conf.colorRenderTargets.size(); i++) {
            const auto dxSwapChain = conf.colorRenderTargets[i].swapChain ?
                static_pointer_cast<DXSwapChain>(conf.colorRenderTargets[i].swapChain) : nullptr;
            const auto dxColorImage = conf.colorRenderTargets[i].renderTarget ?
                static_pointer_cast<DXRenderTarget>(conf.colorRenderTargets[i].renderTarget) : nullptr;
            const auto dxMsaaTarget =
                static_pointer_cast<DXRenderTarget>(conf.colorRenderTargets[i].multisampledRenderTarget);
            if (conf.colorRenderTargets[i].multisampledRenderTarget) {
                rtvHandles[i] = dxMsaaTarget->getHandle();
                resolveSource.push_back(dxMsaaTarget->getImage());
                resolveDestination.push_back(
                    dxSwapChain ? dxSwapChain->getRenderTargets()[dxSwapChain->getCurrentFrameIndex()].Get() :
                    static_pointer_cast<DXImage>(dxColorImage->getImage())->getImage().Get());
            } else if (dxSwapChain) {
                D3D12_CPU_DESCRIPTOR_HANDLE handle;
#ifdef _MSC_VER
                handle = dxSwapChain->getHeap()->GetCPUDescriptorHandleForHeapStart();
#else
                dxSwapChain->getHeap()->GetCPUDescriptorHandleForHeapStart(&handle);
#endif
                rtvHandles[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(
                    handle,
                    dxSwapChain->getCurrentFrameIndex(),
                    dxSwapChain->getDescriptorSize());
            } else if (dxColorImage) {
                rtvHandles[i] = dxColorImage->getHandle();
            }

            if (conf.colorRenderTargets[i].clear && (dxSwapChain || dxColorImage)) {
                const FLOAT clearColor[] = {
                    conf.colorRenderTargets[i].clearValue.color[0],
                    conf.colorRenderTargets[i].clearValue.color[1],
                    conf.colorRenderTargets[i].clearValue.color[2],
                    conf.colorRenderTargets[i].clearValue.color[3]
                };
                commandList->ClearRenderTargetView(rtvHandles[i], clearColor, 0, nullptr);
            }
            if (conf.colorRenderTargets[i].discardAfterRender) {
                colorTargetsToDiscard.push_back(static_pointer_cast<DXImage>(dxColorImage->getImage())->getImage());
            }
        }

        commandList->OMSetRenderTargets(
            rtvHandles.size(),
            rtvHandles.empty() ? nullptr : rtvHandles.data(),
            FALSE,
            haveDepthResource ? &dsvHandle : nullptr);
    }

    void DXCommandList::endRendering() {
        for (int i = 0; i < resolveSource.size(); i++) {
            const auto source = static_pointer_cast<DXImage>(resolveSource[i])->getImage().Get();
            {
                const auto barrier1 = CD3DX12_RESOURCE_BARRIER::Transition(
                       source,
                       D3D12_RESOURCE_STATE_RENDER_TARGET,
                       D3D12_RESOURCE_STATE_RESOLVE_SOURCE);
                commandList->ResourceBarrier(1, &barrier1);
                const auto barrier2 = CD3DX12_RESOURCE_BARRIER::Transition(
                   resolveDestination[i].Get(),
                   D3D12_RESOURCE_STATE_RENDER_TARGET,
                   D3D12_RESOURCE_STATE_RESOLVE_DEST);
                commandList->ResourceBarrier(1, &barrier2);
            }
            commandList->ResolveSubresource(
                resolveDestination[i].Get(),
                0,
                source,
                0,
                DXImage::dxFormats[static_cast<int>(resolveSource[i]->getFormat())]);
            {
                const auto barrier1 = CD3DX12_RESOURCE_BARRIER::Transition(
                       source,
                       D3D12_RESOURCE_STATE_RESOLVE_SOURCE,
                       D3D12_RESOURCE_STATE_RENDER_TARGET);
                commandList->ResourceBarrier(1, &barrier1);
                const auto barrier2 = CD3DX12_RESOURCE_BARRIER::Transition(
                   resolveDestination[i].Get(),
                   D3D12_RESOURCE_STATE_RESOLVE_DEST,
                   D3D12_RESOURCE_STATE_RENDER_TARGET);
                commandList->ResourceBarrier(1, &barrier2);
            }
        }
        resolveSource.clear();
        resolveDestination.clear();

        if (depthTargetToDiscard) {
            commandList->DiscardResource(depthTargetToDiscard.Get(), nullptr);
            depthTargetToDiscard = nullptr;
        }
        for (auto& colorTarget : colorTargetsToDiscard) {
            commandList->DiscardResource(colorTarget.Get(), nullptr);
        }
        colorTargetsToDiscard.clear();
    }

    void DXCommandList::dispatch(const uint32_t x, const uint32_t y, const uint32_t z) const {
        commandList->Dispatch(x, y, z);
    }

    void DXCommandList::barrier(
        const std::shared_ptr<const Image>& image,
        const ResourceState oldState,
        const ResourceState newState) const {
        assert(image != nullptr);
        barrier(static_pointer_cast<const DXImage>(image)->getImage(), oldState, newState);
    }

    void DXCommandList::barrier(
        const std::shared_ptr<const SwapChain>& swapChain,
        const ResourceState oldState,
        const ResourceState newState) const {
        assert(swapChain != nullptr);
        const auto dxSwapChain = static_pointer_cast<const DXSwapChain>(swapChain);
        barrier(dxSwapChain->getRenderTargets()[dxSwapChain->getCurrentFrameIndex()], oldState, newState);
    }

    void DXCommandList::barrier(
       const std::shared_ptr<const RenderTarget>& renderTarget,
       const ResourceState oldState,
       const ResourceState newState) const {
        assert(renderTarget != nullptr);
        barrier( static_pointer_cast<const DXImage>(renderTarget->getImage())->getImage(), oldState, newState);
    }

    void DXCommandList::barrier(
        const std::vector<std::shared_ptr<const RenderTarget>>& renderTargets,
        const ResourceState oldState,
        const ResourceState newState) const {
        assert(renderTargets.size() > 0);
        const auto r = std::views::transform(renderTargets, [](const std::shared_ptr<const RenderTarget>& renderTarget) {
            return static_pointer_cast<const DXImage>(renderTarget->getImage())->getImage().Get();
        });
        barrier(std::vector<ID3D12Resource*>{r.begin(), r.end()}, oldState, newState);
    }

    void DXCommandList::convertState(
            const ResourceState oldState,
            const ResourceState newState,
            D3D12_RESOURCE_STATES& srcState,
            D3D12_RESOURCE_STATES& dstState) {
        if (oldState == ResourceState::UNDEFINED && newState == ResourceState::DISPATCH_TARGET) {
            srcState = D3D12_RESOURCE_STATE_COMMON;
            dstState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
        } else if (oldState == ResourceState::UNDEFINED && newState == ResourceState::RENDER_TARGET_COLOR) {
            srcState = D3D12_RESOURCE_STATE_COMMON;
            dstState = D3D12_RESOURCE_STATE_RENDER_TARGET;
        } else if (oldState == ResourceState::COPY_SRC && newState == ResourceState::RENDER_TARGET_COLOR) {
            srcState = D3D12_RESOURCE_STATE_COPY_SOURCE;
            dstState = D3D12_RESOURCE_STATE_RENDER_TARGET;
        } else if (oldState == ResourceState::RENDER_TARGET_COLOR && newState == ResourceState::COMPUTE_READ) {
            srcState = D3D12_RESOURCE_STATE_RENDER_TARGET;
            dstState = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
        } else if (oldState == ResourceState::RENDER_TARGET_COLOR && newState == ResourceState::SHADER_READ) {
            srcState = D3D12_RESOURCE_STATE_RENDER_TARGET;
            dstState = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
        } else if (oldState == ResourceState::SHADER_READ && newState == ResourceState::UNDEFINED) {
            srcState = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
            dstState = D3D12_RESOURCE_STATE_COMMON;
        } else if (oldState == ResourceState::UNDEFINED && newState == ResourceState::RENDER_TARGET_DEPTH_STENCIL) {
            srcState = D3D12_RESOURCE_STATE_COMMON;
            dstState = D3D12_RESOURCE_STATE_DEPTH_WRITE;
        } else if (oldState == ResourceState::RENDER_TARGET_DEPTH_STENCIL && newState == ResourceState::RENDER_TARGET_DEPTH_STENCIL_READ) {
            srcState = D3D12_RESOURCE_STATE_DEPTH_WRITE;
            dstState = D3D12_RESOURCE_STATE_DEPTH_READ;
        } else if (oldState == ResourceState::COMPUTE_READ && newState == ResourceState::UNDEFINED) {
            srcState = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
            dstState = D3D12_RESOURCE_STATE_COMMON;
        } else if (oldState == ResourceState::COMPUTE_READ && newState == ResourceState::COPY_SRC) {
            srcState = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
            dstState = D3D12_RESOURCE_STATE_COPY_SOURCE;
        } else if (oldState == ResourceState::RENDER_TARGET_COLOR && newState == ResourceState::PRESENT) {
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
        } else if (oldState == ResourceState::RENDER_TARGET_COLOR && newState == ResourceState::COPY_SRC) {
            srcState = D3D12_RESOURCE_STATE_RENDER_TARGET;
            dstState = D3D12_RESOURCE_STATE_COPY_SOURCE;
        } else if (oldState == ResourceState::COPY_SRC && newState == ResourceState::UNDEFINED) {
            srcState = D3D12_RESOURCE_STATE_COPY_SOURCE;
            dstState = D3D12_RESOURCE_STATE_COMMON;
        } else if (oldState == ResourceState::DISPATCH_TARGET && newState == ResourceState::UNDEFINED) {
            srcState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
            dstState = D3D12_RESOURCE_STATE_COMMON;
        } else if (oldState == ResourceState::RENDER_TARGET_COLOR && newState == ResourceState::UNDEFINED) {
            srcState = D3D12_RESOURCE_STATE_RENDER_TARGET;
            dstState = D3D12_RESOURCE_STATE_COMMON;
        } else if (oldState == ResourceState::RENDER_TARGET_DEPTH_STENCIL && newState == ResourceState::UNDEFINED) {
            srcState = D3D12_RESOURCE_STATE_DEPTH_WRITE;
            dstState = D3D12_RESOURCE_STATE_COMMON;
        } else if (oldState == ResourceState::RENDER_TARGET_DEPTH_STENCIL_READ && newState == ResourceState::UNDEFINED) {
            srcState = D3D12_RESOURCE_STATE_DEPTH_READ;
            dstState = D3D12_RESOURCE_STATE_COMMON;
        } else if (oldState == ResourceState::COPY_SRC && newState == ResourceState::DISPATCH_TARGET) {
            srcState = D3D12_RESOURCE_STATE_COPY_SOURCE;
            dstState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
        } else {
            throw Exception("Not implemented");
            return;
        }
    }

    void DXCommandList::barrier(
       const ComPtr<ID3D12Resource>& resource,
       const ResourceState oldState,
       const ResourceState newState) const {
        D3D12_RESOURCE_STATES srcState, dstState;
        convertState(oldState, newState, srcState, dstState);
        const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            resource.Get(),
            srcState,
            dstState);
        commandList->ResourceBarrier(1, &barrier);
    }

    void DXCommandList::barrier(
       const std::vector<ID3D12Resource*>& resources,
       const ResourceState oldState,
       const ResourceState newState) const {
        D3D12_RESOURCE_STATES srcState, dstState;
        convertState(oldState, newState, srcState, dstState);
        std::vector<D3D12_RESOURCE_BARRIER> barriers(resources.size());
        for (int i = 0; i < resources.size(); i++) {
            barriers[i] = CD3DX12_RESOURCE_BARRIER::Transition(resources[i], srcState, dstState);
        }
        commandList->ResourceBarrier(barriers.size(), barriers.data());
    }

    void DXCommandList::pushConstants(
        const std::shared_ptr<const PipelineResources>& pipelineResources,
        const PushConstantsDesc& pushConstants,
        const void* data) const {
        assert(pipelineResources != nullptr);
        assert(data != nullptr);
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
        stagingBuffers.clear();
    }

    void DXCommandList::bindVertexBuffers(const std::vector<std::shared_ptr<const Buffer>>& buffers, const std::vector<size_t> offsets) const {
        assert(buffers.size() > 0);
        assert(buffers.size() == offsets.size());
        std::vector<D3D12_VERTEX_BUFFER_VIEW> bufferViews(buffers.size());
        for (int i = 0; i < buffers.size(); i++) {
            const auto& vertexBuffer = static_pointer_cast<const DXBuffer>(buffers[i]);
            const auto offset = offsets.empty() ? 0 : offsets[i];
            bufferViews[i] = D3D12_VERTEX_BUFFER_VIEW {
                .BufferLocation = vertexBuffer->getBuffer().Get()->GetGPUVirtualAddress() + offset,
                .SizeInBytes = static_cast<UINT>(vertexBuffer->getSize() - offset),
                .StrideInBytes = static_cast<UINT>(vertexBuffer->getStride()),
            };
        }
        commandList->IASetVertexBuffers(0, static_cast<UINT>(buffers.size()), bufferViews.data());
    }

    void DXCommandList::bindVertexBuffer(const std::shared_ptr<const Buffer>& buffer, const size_t offset) const {
        assert(buffer != nullptr);
        const auto& vertexBuffer = static_pointer_cast<const DXBuffer>(buffer);
        const auto bufferView = D3D12_VERTEX_BUFFER_VIEW {
            .BufferLocation = vertexBuffer->getBuffer().Get()->GetGPUVirtualAddress() + offset,
            .SizeInBytes = static_cast<UINT>(vertexBuffer->getSize() - offset),
            .StrideInBytes = static_cast<UINT>(vertexBuffer->getStride()),
        };
        commandList->IASetVertexBuffers(0, 1, &bufferView);
    }

    void DXCommandList::bindIndexBuffer(const std::shared_ptr<const Buffer>& buffer, IndexType indexType, const size_t offset) const {
        assert(buffer != nullptr);
        const auto& indexBuffer = static_pointer_cast<const DXBuffer>(buffer);
        const auto bufferView = D3D12_INDEX_BUFFER_VIEW {
            .BufferLocation = indexBuffer->getBuffer().Get()->GetGPUVirtualAddress() + offset,
            .SizeInBytes = static_cast<UINT>(indexBuffer->getSize() - offset),
            .Format = dxIndexType[static_cast<int>(indexType)]
        };
        commandList->IASetIndexBuffer(&bufferView);
    }

    void DXCommandList::draw(
        const uint32_t vertexCountPerInstance,
        const uint32_t instanceCount,
        const uint32_t firstVertex,
        const uint32_t firstInstance) const {
        commandList->DrawInstanced(vertexCountPerInstance, instanceCount, firstVertex, firstInstance);
    }

    void DXCommandList::drawIndexed(
        const uint32_t indexCountPerInstance,
        const uint32_t instanceCount,
        const uint32_t firstIndex,
        const uint32_t vertexOffset,
        const uint32_t firstInstance) const {
        commandList->DrawIndexedInstanced(indexCountPerInstance, instanceCount, firstIndex, vertexOffset, firstInstance);
    }

    void DXCommandList::upload(const std::shared_ptr<const Buffer>& destination, const void* source) {
        assert(destination != nullptr);
        assert(source != nullptr);
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

    void DXCommandList::upload(
        const std::shared_ptr<const Image>& destination,
        const void* source,
        const uint32_t firstMipLevel) {
        assert(destination != nullptr);
        assert(source != nullptr);
        const auto image = static_pointer_cast<const DXImage>(destination);
        auto stagingBuffer = ComPtr<ID3D12Resource>{nullptr};
        {
            const auto stagingBufferSize = GetRequiredIntermediateSize(
                image->getImage().Get(),
                0,
                1);
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

        const auto resourceIndex = D3D12CalcSubresource(
            firstMipLevel,
            0,
            0,
            image->getMipLevels(),
            1);
        const auto copyData = D3D12_SUBRESOURCE_DATA {
            .pData = source,
            .RowPitch = static_cast<LONG_PTR>(image->getRowPitch()),
            .SlicePitch = static_cast<LONG_PTR>(image->getImageSize()),
        };
        UpdateSubresources(
            commandList.Get(),
            image->getImage().Get(),
            stagingBuffer.Get(),
            0,
            resourceIndex,
            1,
            &copyData);
        stagingBuffers.push_back(stagingBuffer);
    }

    void DXCommandList::copy(
       const std::shared_ptr<const Buffer>& source,
       const std::shared_ptr<const Image>& destination,
       const uint32_t sourceOffset,
       const uint32_t firstMipLevel) {
        assert(source != nullptr);
        assert(destination != nullptr);
        const auto image = static_pointer_cast<const DXImage>(destination);
        const auto buffer = static_pointer_cast<const DXBuffer>(source);

        auto dstLocation = D3D12_TEXTURE_COPY_LOCATION{
            .pResource = image->getImage().Get(),
            .Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
        };
        dstLocation.SubresourceIndex = D3D12CalcSubresource(
            firstMipLevel,
            0,
            0,
            image->getMipLevels(),
            image->getArraySize());

        const auto subresource = dstLocation.SubresourceIndex;
        const auto texDesc = image->getImage()->GetDesc();
        auto footprint = D3D12_PLACED_SUBRESOURCE_FOOTPRINT{};
        UINT64 totalBytes{0};
        device->GetCopyableFootprints(
            &texDesc,
            subresource,
            1,
            sourceOffset,
            &footprint,
            nullptr,
            nullptr,
            &totalBytes);

        const auto srcLocation = D3D12_TEXTURE_COPY_LOCATION{
            .pResource = buffer->getBuffer().Get(),
            .Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT,
            .PlacedFootprint = footprint,
        };

        commandList->CopyTextureRegion(
            &dstLocation,
            0, 0, 0,
            &srcLocation,
            nullptr
        );
    }

    void DXCommandList::uploadArray(
        const std::shared_ptr<const Image>& destination,
        const std::vector<void*>& sources,
        const uint32_t firstMipLevel) {
        assert(destination != nullptr);
        assert(sources.size() == destination->getArraySize());
        const auto image = static_pointer_cast<const DXImage>(destination);

        auto stagingBuffer = ComPtr<ID3D12Resource>{nullptr};
        {
            const auto stagingBufferSize = GetRequiredIntermediateSize(
                image->getImage().Get(),
                0,
                sources.size());
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
            const auto resourceIndex = D3D12CalcSubresource(
                   firstMipLevel,
                   0,
                   0,
                   image->getMipLevels(),
                   image->getArraySize());
            auto copyData = std::vector<D3D12_SUBRESOURCE_DATA>(sources.size());
            for (int i = 0; i < sources.size(); i++) {
                copyData[i].pData = sources[i];
                copyData[i].RowPitch = static_cast<LONG_PTR>(image->getRowPitch());
                copyData[i].SlicePitch = static_cast<LONG_PTR>(image->getImageSize());
            }
            UpdateSubresources(
                commandList.Get(),
                image->getImage().Get(),
                stagingBuffer.Get(),
                0,
                resourceIndex,
                copyData.size(),
                copyData.data());
        }
        stagingBuffers.push_back(stagingBuffer);
    }

    void DXCommandList::copy(
        const std::shared_ptr<const Image>& source,
        const std::shared_ptr<const SwapChain>& swapChain) const {
        assert(source != nullptr);
        assert(swapChain != nullptr);
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
       const std::shared_ptr<const Image>& source,
       const std::shared_ptr<const SwapChain>& swapChain,
       Filter filter) const {
       copy(source, swapChain);
    }

}
