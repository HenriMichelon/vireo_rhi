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
        const std::shared_ptr<const SwapChain>&swapChain,
        const std::vector<std::shared_ptr<const CommandList>>& commandLists) const {
        assert(fence != nullptr);
        assert(swapChain != nullptr);
        submit(commandLists);
        const auto dxFence = static_pointer_cast<DXFence>(fence);
        const auto dxSwapChain = static_pointer_cast<const DXSwapChain>(swapChain);
        dxFence->setValue(dxSwapChain->getFenceValue());
    }

    void DXSubmitQueue::submit(
        const std::shared_ptr<Fence>& fence,
        const std::vector<std::shared_ptr<const CommandList>>& commandLists) const {
        assert(fence != nullptr);
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

    void DXSubmitQueue::submit(
        const std::shared_ptr<Semaphore>& waitSemaphore,
        const WaitStage,
        const WaitStage,
        const std::shared_ptr<Semaphore>& signalSemaphore,
        const std::vector<std::shared_ptr<const CommandList>>& commandLists) const {
        assert(waitSemaphore != nullptr || signalSemaphore != nullptr);
        const auto dxWaitSemaphore = static_pointer_cast<DXSemaphore>(waitSemaphore);
        const auto dxSignalSemaphore = static_pointer_cast<DXSemaphore>(signalSemaphore);
        if (dxWaitSemaphore) {
            dxCheck(commandQueue->Wait(dxWaitSemaphore->getFence().Get(), dxWaitSemaphore->getValue()));
        }
        submit(commandLists);
        if (dxSignalSemaphore) {
            dxSignalSemaphore->incrementValue();
            dxCheck(commandQueue->Signal(dxSignalSemaphore->getFence().Get(), dxSignalSemaphore->getValue()));
        }
    }

    void DXSubmitQueue::submit(
        const std::shared_ptr<Semaphore>& waitSemaphore,
        const std::vector<WaitStage>& waitStages,
        const WaitStage,
        const std::shared_ptr<Semaphore>& signalSemaphore,
        const std::vector<std::shared_ptr<const CommandList>>& commandLists) const {
        assert(waitSemaphore != nullptr || signalSemaphore != nullptr);
        const auto dxWaitSemaphore = static_pointer_cast<DXSemaphore>(waitSemaphore);
        const auto dxSignalSemaphore = static_pointer_cast<DXSemaphore>(signalSemaphore);
        if (dxWaitSemaphore) {
            assert(waitSemaphore->getType() == SemaphoreType::TIMELINE);
            assert(waitStages.size() > 0);
            for (int i = 0; i < waitStages.size(); i++) {
                dxCheck(commandQueue->Wait(dxWaitSemaphore->getFence().Get(), dxWaitSemaphore->getValue() + i));
            }
        }
        submit(commandLists);
        if (dxSignalSemaphore) {
            dxSignalSemaphore->incrementValue();
            dxCheck(commandQueue->Signal(dxSignalSemaphore->getFence().Get(), dxSignalSemaphore->getValue()));
        }
    }

    void DXSubmitQueue::submit(
        const std::shared_ptr<Semaphore>& waitSemaphore,
        const WaitStage waitStage,
        const std::shared_ptr<Fence>& fence,
        const std::shared_ptr<const SwapChain>&swapChain,
        const std::vector<std::shared_ptr<const CommandList>>& commandLists) const {
        submit(waitSemaphore, waitStage, WaitStage::NONE, nullptr, commandLists);
        const auto dxFence = static_pointer_cast<DXFence>(fence);
        const auto dxSwapChain = static_pointer_cast<const DXSwapChain>(swapChain);
        dxFence->setValue(dxSwapChain->getFenceValue());
    }

    void DXSubmitQueue::submit(
           const std::shared_ptr<Semaphore>& waitSemaphore,
           const std::vector<WaitStage>& waitStages,
           const std::shared_ptr<Fence>& fence,
           const std::shared_ptr<const SwapChain>&swapChain,
           const std::vector<std::shared_ptr<const CommandList>>& commandLists) const {
        submit(waitSemaphore, waitStages, WaitStage::NONE, nullptr, commandLists);
        const auto dxFence = static_pointer_cast<DXFence>(fence);
        const auto dxSwapChain = static_pointer_cast<const DXSwapChain>(swapChain);
        dxFence->setValue(dxSwapChain->getFenceValue());
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

    std::shared_ptr<CommandList> DXCommandAllocator::createCommandList(const Pipeline& pipeline) const {
        return std::make_shared<DXCommandList>(
            getCommandListType(),
            device,
            commandAllocator,
            static_cast<const DXGraphicPipeline&>(pipeline).getPipelineState());
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
        DXCommandList::cleanup();
    }

    void DXCommandList::bindPipeline(const Pipeline& pipeline) {
        if (pipeline.getType() == PipelineType::COMPUTE) {
            commandList->SetPipelineState(static_cast<const DXComputePipeline&>(pipeline).getPipelineState().Get());
            commandList->SetComputeRootSignature(static_pointer_cast<const DXPipelineResources>(pipeline.getResources())->getRootSignature().Get());
        } else {
            const auto& dxPipeline = static_cast<const DXGraphicPipeline&>(pipeline);
            commandList->SetPipelineState(dxPipeline.getPipelineState().Get());
            commandList->SetGraphicsRootSignature(static_pointer_cast<const DXPipelineResources>(pipeline.getResources())->getRootSignature().Get());
            commandList->IASetPrimitiveTopology(dxPipeline.getPrimitiveTopology());
        }
    }

    void DXCommandList::setDescriptors(
        const std::vector<std::shared_ptr<const DescriptorSet>>& descriptors) const {
        assert(descriptors.size() > 0);
        std::vector<ID3D12DescriptorHeap*> heaps(descriptors.size());
        for (int i = 0; i < descriptors.size(); i++) {
            heaps[i] = static_pointer_cast<const DXDescriptorSet>(descriptors[i])->getHeap().Get();
        }
        commandList->SetDescriptorHeaps(heaps.size(), heaps.data());
    }

    void DXCommandList::bindDescriptors(
        const Pipeline& pipeline,
        const std::vector<std::shared_ptr<const DescriptorSet>>& descriptors,
        const uint32_t firstSet) const {
        assert(descriptors.size() > 0);
        D3D12_GPU_DESCRIPTOR_HANDLE handle;
        for (int i = 0; i < descriptors.size(); i++) {
            const auto heap =  static_pointer_cast<const DXDescriptorSet>(descriptors[i])->getHeap().Get();
#ifdef _MSC_VER
            handle = heap->GetGPUDescriptorHandleForHeapStart();
#else
            heap->GetGPUDescriptorHandleForHeapStart(&handle);
#endif
            if (pipeline.getType() == PipelineType::COMPUTE) {
                commandList->SetComputeRootDescriptorTable(firstSet + i, handle);
            } else {
                commandList->SetGraphicsRootDescriptorTable(firstSet + i, handle);
            }
        }
    }

    void DXCommandList::bindDescriptor(
        const Pipeline&pipeline,
        const DescriptorSet& descriptor,
        const uint32_t set) const {
        const auto heap = static_cast<const DXDescriptorSet&>(descriptor).getHeap().Get();
        D3D12_GPU_DESCRIPTOR_HANDLE handle;
#ifdef _MSC_VER
        handle = heap->GetGPUDescriptorHandleForHeapStart();
#else
        heap->GetGPUDescriptorHandleForHeapStart(&handle);
#endif
        if (pipeline.getType() == PipelineType::COMPUTE) {
            commandList->SetComputeRootDescriptorTable(set, handle);
        } else {
            commandList->SetGraphicsRootDescriptorTable(set, handle);
        }
    }

    void DXCommandList::bindDescriptor(
        const Pipeline& pipeline,
        const DescriptorSet& descriptor,
        const uint32_t set,
        const uint32_t offset) const {
        assert(descriptor.getLayout()->isDynamicUniform());
        const auto& dxDescriptorSet = static_cast<const DXDescriptorSet&>(descriptor);
        const auto& buffer = static_pointer_cast<const DXBuffer>(dxDescriptorSet.getDynamicBuffer());
        assert(buffer->getType() == BufferType::UNIFORM);
        commandList->SetGraphicsRootConstantBufferView(
            set,
            buffer->getBuffer()->GetGPUVirtualAddress() + offset);
    }

    void DXCommandList::setViewports(const std::vector<Viewport>& viewports) const {
        std::vector<CD3DX12_VIEWPORT> dxViewports(viewports.size());
        for (int i = 0; i < viewports.size(); i++) {
            dxViewports[i].TopLeftX = viewports[i].x;
            dxViewports[i].TopLeftY = viewports[i].height - viewports[i].y;
            dxViewports[i].Width = viewports[i].width;
            dxViewports[i].Height = -viewports[i].height;
            dxViewports[i].MinDepth = viewports[i].minDepth;
            dxViewports[i].MaxDepth = viewports[i].maxDepth;
        }
        commandList->RSSetViewports(dxViewports.size(), dxViewports.data());
    }

    void DXCommandList::setScissors(const std::vector<Rect>& rect) const {
        std::vector<CD3DX12_RECT> scissors(rect.size());
        for (int i = 0; i < scissors.size(); i++) {
            scissors[i].left = rect[i].x;
            scissors[i].top = rect[i].y;
            scissors[i].right = rect[i].width;
            scissors[i].bottom = rect[i].height;
        }
        commandList->RSSetScissorRects(scissors.size(), scissors.data());
    }

    void DXCommandList::setViewport(const Viewport& viewport) const {
        const auto dxViewport = D3D12_VIEWPORT{
            .TopLeftX = viewport.x,
            .TopLeftY = viewport.y,
            .Width = viewport.width,
            .Height = viewport.height,
            .MinDepth = viewport.minDepth,
            .MaxDepth = viewport.maxDepth
        };
        commandList->RSSetViewports(1, &dxViewport);
    }

    void DXCommandList::setScissors(const Rect& rect) const {
        const auto scissor = D3D12_RECT{
            .left = rect.x,
            .top = rect.y,
            .right = static_cast<LONG>(rect.width),
            .bottom = static_cast<LONG>(rect.height),
        };
        commandList->RSSetScissorRects(1, &scissor);
    }

    void DXCommandList::beginRendering(const RenderingConfiguration& conf) {
        const auto dxDepthImage =
            conf.depthStencilRenderTarget ? static_pointer_cast<DXRenderTarget>(conf.depthStencilRenderTarget) : nullptr;
        D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle{};
        if (conf.multisampledDepthStencilRenderTarget) {
            const auto dxMsaaTarget = static_pointer_cast<DXRenderTarget>(conf.multisampledDepthStencilRenderTarget);
            dsvHandle = dxMsaaTarget->getHandle();
        } else if (dxDepthImage) {
            dsvHandle = dxDepthImage->getHandle();
        }
        const bool haveDepthResource = dxDepthImage || conf.multisampledDepthStencilRenderTarget;
        if (conf.clearDepthStencil && haveDepthResource) {
            commandList->ClearDepthStencilView(
                dsvHandle,
                D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
                conf.depthStencilClearValue.depthStencil.depth, conf.depthStencilClearValue.depthStencil.stencil,
                0, nullptr);
            if (conf.discardDepthStencilAfterRender) {
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

    void DXCommandList::setStencilReference(const uint32_t reference) const {
        commandList->OMSetStencilRef(reference);
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
        const ResourceState newState,
        const uint32_t firstMipLevel,
        const uint32_t levelCount) const {
        assert(image != nullptr);
        barrier(
            static_pointer_cast<const DXImage>(image)->getImage(),
            oldState, newState,
            firstMipLevel, levelCount,
            image->getArraySize());
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
        } else if (oldState == ResourceState::SHADER_READ && newState == ResourceState::RENDER_TARGET_COLOR) {
            srcState = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
            dstState = D3D12_RESOURCE_STATE_RENDER_TARGET;
        } else if (oldState == ResourceState::RENDER_TARGET_COLOR && newState == ResourceState::COMPUTE_READ) {
            srcState = D3D12_RESOURCE_STATE_RENDER_TARGET;
            dstState = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
        } else if (oldState == ResourceState::UNDEFINED && newState == ResourceState::SHADER_READ) {
            srcState = D3D12_RESOURCE_STATE_COMMON;
            dstState = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
        } else if (oldState == ResourceState::RENDER_TARGET_COLOR && newState == ResourceState::SHADER_READ) {
            srcState = D3D12_RESOURCE_STATE_RENDER_TARGET;
            dstState = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
        } else if (oldState == ResourceState::SHADER_READ && newState == ResourceState::UNDEFINED) {
            srcState = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
            dstState = D3D12_RESOURCE_STATE_COMMON;
        } else if (oldState == ResourceState::UNDEFINED &&
                (newState == ResourceState::RENDER_TARGET_DEPTH_STENCIL || newState == ResourceState::RENDER_TARGET_DEPTH)) {
            srcState = D3D12_RESOURCE_STATE_COMMON;
            dstState = D3D12_RESOURCE_STATE_DEPTH_WRITE;
        } else if ((oldState == ResourceState::RENDER_TARGET_DEPTH_STENCIL || oldState == ResourceState::RENDER_TARGET_DEPTH) &&
                (newState == ResourceState::RENDER_TARGET_DEPTH_STENCIL_READ || newState == ResourceState::RENDER_TARGET_DEPTH_READ)) {
            srcState = D3D12_RESOURCE_STATE_DEPTH_WRITE;
            dstState = D3D12_RESOURCE_STATE_DEPTH_READ;
        } else if ((oldState == ResourceState::RENDER_TARGET_DEPTH_STENCIL_READ || oldState == ResourceState::RENDER_TARGET_DEPTH_READ) &&
                (newState == ResourceState::RENDER_TARGET_DEPTH_STENCIL || newState == ResourceState::RENDER_TARGET_DEPTH)) {
            srcState = D3D12_RESOURCE_STATE_DEPTH_READ;
            dstState = D3D12_RESOURCE_STATE_DEPTH_WRITE;
        } else if (oldState == ResourceState::UNDEFINED &&
                (newState == ResourceState::RENDER_TARGET_DEPTH_STENCIL_READ || newState == ResourceState::RENDER_TARGET_DEPTH_READ)) {
            srcState = D3D12_RESOURCE_STATE_COMMON;
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
        } else if (oldState == ResourceState::COPY_SRC && newState == ResourceState::SHADER_READ) {
            srcState = D3D12_RESOURCE_STATE_COPY_SOURCE;
            dstState = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
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
        } else if ((oldState == ResourceState::RENDER_TARGET_DEPTH_STENCIL || oldState == ResourceState::RENDER_TARGET_DEPTH) &&
                newState == ResourceState::UNDEFINED) {
            srcState = D3D12_RESOURCE_STATE_DEPTH_WRITE;
            dstState = D3D12_RESOURCE_STATE_COMMON;
        } else if ((oldState == ResourceState::RENDER_TARGET_DEPTH_STENCIL_READ || oldState == ResourceState::RENDER_TARGET_DEPTH_READ) &&
                newState == ResourceState::UNDEFINED) {
            srcState = D3D12_RESOURCE_STATE_DEPTH_READ;
            dstState = D3D12_RESOURCE_STATE_COMMON;
        } else if (oldState == ResourceState::COPY_SRC && newState == ResourceState::DISPATCH_TARGET) {
            srcState = D3D12_RESOURCE_STATE_COPY_SOURCE;
            dstState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
        } else if (oldState == ResourceState::COPY_SRC && newState == ResourceState::COPY_DST) {
            srcState = D3D12_RESOURCE_STATE_COPY_SOURCE;
            dstState = D3D12_RESOURCE_STATE_COPY_DEST;
        } else if (oldState == ResourceState::COPY_DST && newState == ResourceState::COPY_SRC) {
            srcState = D3D12_RESOURCE_STATE_COPY_DEST;
            dstState = D3D12_RESOURCE_STATE_COPY_SOURCE;
        } else if (oldState == ResourceState::SHADER_READ && newState == ResourceState::COPY_SRC) {
            srcState = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
            dstState = D3D12_RESOURCE_STATE_COPY_SOURCE;
        } else if (oldState == ResourceState::COPY_DST && newState == ResourceState::UNDEFINED) {
            srcState = D3D12_RESOURCE_STATE_COPY_DEST;
            dstState = D3D12_RESOURCE_STATE_COMMON;
        } else {
            throw Exception("Not implemented");
            return;
        }
    }

    void DXCommandList::barrier(
        const ComPtr<ID3D12Resource>& resource,
        const ResourceState oldState,
        const ResourceState newState,
        const uint32_t firstMipLevel,
        const uint32_t levelCount,
        const uint32_t arraySize) const {
        D3D12_RESOURCE_STATES srcState, dstState;
        convertState(oldState, newState, srcState, dstState);
        std::vector<D3D12_RESOURCE_BARRIER> barriers;
        if (oldState == ResourceState::RENDER_TARGET_DEPTH_STENCIL ||
            newState == ResourceState::RENDER_TARGET_DEPTH_STENCIL ||
            oldState == ResourceState::RENDER_TARGET_DEPTH_STENCIL_READ ||
            newState == ResourceState::RENDER_TARGET_DEPTH_STENCIL_READ) {
            for (int plane = 0; plane < 2; ++plane) {
                const auto subresourceIndex = D3D12CalcSubresource(
                    0,
                    0,
                    plane,
                    levelCount,
                    arraySize);
                barriers.push_back(CD3DX12_RESOURCE_BARRIER::Transition(
                    resource.Get(),
                    srcState,
                    dstState,
                    subresourceIndex));
            }
        } else {
            barriers.reserve(arraySize);
            for (int slice = 0; slice < arraySize; ++slice) {
                for (int mip = firstMipLevel; mip < levelCount; ++mip) {
                    const auto subresourceIndex = D3D12CalcSubresource(
                        mip,
                        slice,
                        0,
                        levelCount,
                        arraySize);
                    barriers.push_back(CD3DX12_RESOURCE_BARRIER::Transition(
                        resource.Get(),
                        srcState,
                        dstState,
                        subresourceIndex));
                }
            }
        }
        commandList->ResourceBarrier(barriers.size(), barriers.data());
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

    void DXCommandList::bindVertexBuffer(const Buffer& buffer, const size_t offset) const {
        const auto& vertexBuffer = static_cast<const DXBuffer&>(buffer);
        const auto bufferView = D3D12_VERTEX_BUFFER_VIEW {
            .BufferLocation = vertexBuffer.getBuffer().Get()->GetGPUVirtualAddress() + offset,
            .SizeInBytes = static_cast<UINT>(vertexBuffer.getSize() - offset),
            .StrideInBytes = static_cast<UINT>(vertexBuffer.getStride()),
        };
        commandList->IASetVertexBuffers(0, 1, &bufferView);
    }

    void DXCommandList::bindIndexBuffer(const Buffer& buffer, IndexType indexType, const uint32_t firstIndex) const {
        const auto& indexBuffer = static_cast<const DXBuffer&>(buffer);
        const auto offset = firstIndex * indexTypeSize[static_cast<int>(indexType)];
        const auto bufferView = D3D12_INDEX_BUFFER_VIEW {
            .BufferLocation = indexBuffer.getBuffer().Get()->GetGPUVirtualAddress() + offset,
            .SizeInBytes = static_cast<UINT>(indexBuffer.getSize() - offset),
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

    void DXCommandList::drawIndirect(
        const Buffer& buffer,
        const size_t offset,
        const uint32_t drawCount,
        const uint32_t stride) {
        checkIndirectCommandSignature(argDesc, stride);
        const auto& dxBuffer = static_cast<const DXBuffer&>(buffer);
        commandList->ExecuteIndirect(
            drawIndirectCommandSignatures.at(stride).Get(),
            drawCount,
            dxBuffer.getBuffer().Get(),
            offset,
            nullptr,
            0
        );
    }
    void DXCommandList::checkIndirectCommandSignature(const D3D12_INDIRECT_ARGUMENT_DESC& argDesc, const uint32_t stride) {
        if (!drawIndirectCommandSignatures.contains(stride)) {
            const D3D12_COMMAND_SIGNATURE_DESC sigDesc {
                .ByteStride = stride,
                .NumArgumentDescs = 1,
                .pArgumentDescs = &argDesc,
            };
            auto commandSignature = ComPtr<ID3D12CommandSignature>{};
            dxCheck(device->CreateCommandSignature(&sigDesc, nullptr, IID_PPV_ARGS(&commandSignature)));
            drawIndirectCommandSignatures[stride] = commandSignature;
        }
    }

    void DXCommandList::drawIndexedIndirect(
        const Buffer& buffer,
        const size_t offset,
        const uint32_t drawCount,
        const uint32_t stride) {
        checkIndirectCommandSignature(argDescIndexed, stride);
        const auto& dxBuffer = static_cast<const DXBuffer&>(buffer);
        commandList->ExecuteIndirect(
            drawIndirectCommandSignatures.at(stride).Get(),
            drawCount,
            dxBuffer.getBuffer().Get(),
            offset,
            nullptr,
            0
        );
    }

    void DXCommandList::drawIndexedIndirectCount(
        Buffer& buffer,
        const size_t offset,
        Buffer& countBuffer,
        const size_t countOffset,
        const uint32_t maxDrawCount,
        const uint32_t stride) {
        checkIndirectCommandSignature(argDescIndexed, stride);
        const auto& dxBuffer = static_cast<const DXBuffer&>(buffer);
        const auto& dxCountBuffer = static_cast<const DXBuffer&>(countBuffer);
        commandList->ExecuteIndirect(
            drawIndirectCommandSignatures.at(stride).Get(),
            maxDrawCount,
            dxBuffer.getBuffer().Get(),
            offset,
            dxCountBuffer.getBuffer().Get(),
            countOffset
        );
    }

    void DXCommandList::upload(const Buffer& destination, const void* source) {
        assert(source != nullptr);
        const auto& buffer = static_cast<const DXBuffer&>(destination);

        ComPtr<ID3D12Resource> stagingBuffer;
        {
            const auto stagingBufferSize = GetRequiredIntermediateSize(buffer.getBuffer().Get(), 0, 1);
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
        }

        {
            const auto memoryBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
                buffer.getBuffer().Get(),
                D3D12_RESOURCE_STATE_COPY_DEST,
                DXBuffer::ResourceStates[static_cast<int>(buffer.getType())]);
            commandList->ResourceBarrier(1, &memoryBarrier);
        }

        stagingBuffers.push_back(stagingBuffer);
    }

    void DXCommandList::copy(
        const Buffer& source,
        const Buffer& destination,
        const size_t size,
        const uint32_t sourceOffset,
        const uint32_t destinationOffset) {
        const auto copySize = size == Buffer::WHOLE_SIZE ? destination.getSize() : size;
        assert(source.getSize() >= (copySize + sourceOffset));
        assert(destination.getSize() >= (copySize + destinationOffset));
        const auto& dxDestination = static_cast<const DXBuffer&>(destination);
        commandList->CopyBufferRegion(
            dxDestination.getBuffer().Get(),
            destinationOffset,
            static_cast<const DXBuffer&>(source).getBuffer().Get(),
            sourceOffset,
            copySize
        );
        const auto memoryBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
            dxDestination.getBuffer().Get(),
            D3D12_RESOURCE_STATE_COPY_DEST,
            DXBuffer::ResourceStates[static_cast<int>(dxDestination.getType())]);
        commandList->ResourceBarrier(1, &memoryBarrier);
    }

    void DXCommandList::copy(
        const Buffer& source,
        const Buffer& destination,
        const std::vector<BufferCopyRegion>& regions) {
        const auto& dxDestination = static_cast<const DXBuffer&>(destination);
        for (const auto& region : regions) {
            commandList->CopyBufferRegion(
               dxDestination.getBuffer().Get(),
               region.dstOffset,
               static_cast<const DXBuffer&>(source).getBuffer().Get(),
               region.srcOffset,
               region.size);
        }
        const auto memoryBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
            dxDestination.getBuffer().Get(),
            D3D12_RESOURCE_STATE_COPY_DEST,
            DXBuffer::ResourceStates[static_cast<int>(dxDestination.getType())]);
            commandList->ResourceBarrier(1, &memoryBarrier);
    }

    void DXCommandList::upload(
        const Image& destination,
        const void* source,
        const uint32_t firstMipLevel) {
        assert(source != nullptr);
        const auto& image = static_cast<const DXImage&>(destination);
        auto stagingBuffer = ComPtr<ID3D12Resource>{nullptr};
        {
            const auto stagingBufferSize = GetRequiredIntermediateSize(
                image.getImage().Get(),
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
            image.getMipLevels(),
            1);
        const auto copyData = D3D12_SUBRESOURCE_DATA {
            .pData = source,
            .RowPitch = static_cast<LONG_PTR>(image.getRowPitch()),
            .SlicePitch = static_cast<LONG_PTR>(image.getImageSize()),
        };
        UpdateSubresources(
            commandList.Get(),
            image.getImage().Get(),
            stagingBuffer.Get(),
            0,
            resourceIndex,
            1,
            &copyData);
        stagingBuffers.push_back(stagingBuffer);
    }

    void DXCommandList::copy(
       const Buffer& source,
       const Image& destination,
       const uint32_t sourceOffset,
       const uint32_t firstMipLevel) {
        const auto& image = static_cast<const DXImage&>(destination);
        const auto& buffer = static_cast<const DXBuffer&>(source);

        auto dstLocation = D3D12_TEXTURE_COPY_LOCATION{
            .pResource = image.getImage().Get(),
            .Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
        };
        dstLocation.SubresourceIndex = D3D12CalcSubresource(
            firstMipLevel,
            0,
            0,
            image.getMipLevels(),
            image.getArraySize());

        const auto subresource = dstLocation.SubresourceIndex;
        const auto texDesc = image.getImage()->GetDesc();
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
            .pResource = buffer.getBuffer().Get(),
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

    void DXCommandList::copy(
        const Image& source,
        const Buffer& destination,
        const uint32_t destinationOffset,
        const uint32_t firstMipLevel) {
        const auto& image = static_cast<const DXImage&>(source);
        const auto& buffer = static_cast<const DXBuffer&>(destination);

        auto srcLocation = D3D12_TEXTURE_COPY_LOCATION{
            .pResource = image.getImage().Get(),
            .Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
        };
        srcLocation.SubresourceIndex = D3D12CalcSubresource(
            firstMipLevel,
            0,
            0,
            image.getMipLevels(),
            image.getArraySize());

        UINT64 totalBytes = 0;
        const auto texDesc = image.getImage()->GetDesc();
        auto footprint = D3D12_PLACED_SUBRESOURCE_FOOTPRINT{};
        device->GetCopyableFootprints(
            &texDesc,
            srcLocation.SubresourceIndex,
            1,
            destinationOffset,
            &footprint,
            nullptr,
            nullptr,
            &totalBytes);

        const auto dstLocation = D3D12_TEXTURE_COPY_LOCATION{
            .pResource = buffer.getBuffer().Get(),
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
        const Image& destination,
        const std::vector<void*>& sources,
        const uint32_t firstMipLevel) {
        assert(sources.size() == destination.getArraySize());
        const auto& image = static_cast<const DXImage&>(destination);

        auto stagingBuffer = ComPtr<ID3D12Resource>{nullptr};
        {
            const auto stagingBufferSize = GetRequiredIntermediateSize(
                image.getImage().Get(),
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
                   image.getMipLevels(),
                   image.getArraySize());
            auto copyData = std::vector<D3D12_SUBRESOURCE_DATA>(sources.size());
            for (int i = 0; i < sources.size(); i++) {
                copyData[i].pData = sources[i];
                copyData[i].RowPitch = static_cast<LONG_PTR>(image.getRowPitch());
                copyData[i].SlicePitch = static_cast<LONG_PTR>(image.getImageSize());
            }
            UpdateSubresources(
                commandList.Get(),
                image.getImage().Get(),
                stagingBuffer.Get(),
                0,
                resourceIndex,
                copyData.size(),
                copyData.data());
        }
        stagingBuffers.push_back(stagingBuffer);
    }

    void DXCommandList::copy(
        const Image& source,
        const SwapChain& swapChain) const {
        const auto& dxSource = static_cast<const DXImage&>(source);
        const auto& dxSwapChain = static_cast<const DXSwapChain&>(swapChain);

        const auto srcLocation = D3D12_TEXTURE_COPY_LOCATION {
            .pResource = dxSource.getImage().Get(),
            .Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
            .SubresourceIndex = 0,
        };
        const auto dstLocation = D3D12_TEXTURE_COPY_LOCATION {
            .pResource = dxSwapChain.getRenderTargets()[dxSwapChain.getCurrentFrameIndex()].Get(),
            .Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
            .SubresourceIndex = 0,
        };

        commandList->CopyTextureRegion(&dstLocation, 0, 0, 0, &srcLocation, nullptr);
    }

    DXFence::DXFence(const ComPtr<ID3D12Device>& device) {
        dxCheck(device->CreateFence(
            0,
            D3D12_FENCE_FLAG_NONE,
            IID_PPV_ARGS(&fence)));
        fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    }

    void DXFence::wait() const {
        if (fence->GetCompletedValue() < fenceValue) {
            dxCheck(fence->SetEventOnCompletion(fenceValue, fenceEvent));
            dxCheck(WaitForSingleObject(fenceEvent, INFINITE));
        }
    }

    DXFence::~DXFence() {
        CloseHandle(fenceEvent);
    }

    DXSemaphore::DXSemaphore(const ComPtr<ID3D12Device>& device, SemaphoreType type):
        Semaphore{type} {
        dxCheck(device->CreateFence(value, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
        if (type == SemaphoreType::BINARY) {
            incrementValue();
        }
    }

}
