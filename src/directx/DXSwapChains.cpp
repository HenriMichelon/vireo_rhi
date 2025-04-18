/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/backend/directx/Libraries.h"
module vireo.directx.swapchains;

import vireo.tools;

import vireo.directx.commands;
import vireo.directx.tools;

namespace vireo {

    DXSwapChain::DXSwapChain(
        const ComPtr<IDXGIFactory4>& factory,
        const shared_ptr<DXDevice>& dxdevice,
        const ComPtr<ID3D12CommandQueue>& commandQueue,
        const HWND hWnd,
        const PresentMode vSyncMode) :
        device{dxdevice},
        factory{factory},
        presentCommandQueue{commandQueue},
        hWnd{hWnd},
        syncInterval{static_cast<UINT>(vSyncMode == PresentMode::IMMEDIATE ? 0 : 1)},
        presentFlags{static_cast<UINT>(vSyncMode == PresentMode::IMMEDIATE ? DXGI_PRESENT_ALLOW_TEARING : 0)} {
        create();
        fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (fenceEvent == nullptr) {
            dxCheck(HRESULT_FROM_WIN32(GetLastError()));
        }
    }

    void DXSwapChain::create() {
        RECT windowRect{};
        if (GetClientRect(hWnd, &windowRect) == 0) {
            throw Exception("Error getting window rect");
            return;
        }
        const auto width = windowRect.right - windowRect.left;
        const auto height = windowRect.bottom - windowRect.top;
        extent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
        aspectRatio = static_cast<float>(extent.width) / static_cast<float>(extent.height);

        const auto swapChainDesc = DXGI_SWAP_CHAIN_DESC1{
            .Width = static_cast<UINT>(width),
            .Height = static_cast<UINT>(height),
            .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
            .SampleDesc = {.Count = 1},
            .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
            .BufferCount = FRAMES_IN_FLIGHT,
            .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
            .Flags = presentFlags == DXGI_PRESENT_ALLOW_TEARING ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0u,
        };

        ComPtr<IDXGISwapChain1> swapChain1;
        dxCheck(factory->CreateSwapChainForHwnd(
            presentCommandQueue.Get(),
            // Swap chain needs the queue so that it can force a flush on it.
            hWnd,
            &swapChainDesc,
            nullptr,
            nullptr,
            &swapChain1));
        dxCheck(swapChain1.As(&swapChain));
        currentFrameIndex = swapChain->GetCurrentBackBufferIndex();

        // Describe and create a render target view (RTV) descriptor heap.
        constexpr auto rtvHeapDesc = D3D12_DESCRIPTOR_HEAP_DESC{
            .Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
            .NumDescriptors = FRAMES_IN_FLIGHT,
            .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
        };
        dxCheck(device->getDevice()->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap)));
        rtvDescriptorSize = device->getDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
#ifdef _DEBUG
        rtvHeap->SetName(L"SwapChain Heap");
#endif

        // Create frame resources.
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());
        constexpr auto rtvDesc = D3D12_RENDER_TARGET_VIEW_DESC{
            .Format = RENDER_FORMAT,
            .ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D,
        };
        for (UINT n = 0; n < FRAMES_IN_FLIGHT; n++) {
            dxCheck(swapChain->GetBuffer(n, IID_PPV_ARGS(&renderTargets[n])));
#ifdef _DEBUG
            renderTargets[n]->SetName(L"SwapChain BackBuffer " + n);
#endif
            device->getDevice()->CreateRenderTargetView(renderTargets[n].Get(), &rtvDesc, rtvHandle);
            rtvHandle.Offset(1, rtvDescriptorSize);
        }
    }

    DXSwapChain::~DXSwapChain() {
        // waitForLastPresentedFrame();
        for (UINT i = 0; i < FRAMES_IN_FLIGHT; ++i) {
            renderTargets[i].Reset();
        }
        rtvHeap.Reset();
        swapChain.Reset();
        CloseHandle(fenceEvent);
    }

    void DXSwapChain::recreate() {
        RECT windowRect{};
        if (GetClientRect(hWnd, &windowRect) == 0) {
            throw Exception("Error getting window rect");
        }
        const auto width = windowRect.right - windowRect.left;
        const auto height = windowRect.bottom - windowRect.top;
        if (width != extent.width || height != extent.height) {
            // waitForLastPresentedFrame();
            extent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
            aspectRatio = static_cast<float>(extent.width) / static_cast<float>(extent.height);

            for (UINT i = 0; i < FRAMES_IN_FLIGHT; ++i) {
                renderTargets[i].Reset();
            }

            auto swapDesc = DXGI_SWAP_CHAIN_DESC{};
            dxCheck(swapChain->GetDesc(&swapDesc));

            dxCheck(swapChain->ResizeBuffers(
                FRAMES_IN_FLIGHT,
                width,
                height,
                swapDesc.BufferDesc.Format,
                swapDesc.Flags
            ));

            CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());
            constexpr auto rtvDesc = D3D12_RENDER_TARGET_VIEW_DESC{
                .Format = RENDER_FORMAT,
                .ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D,
            };
            for (UINT n = 0; n < FRAMES_IN_FLIGHT; n++) {
                dxCheck(swapChain->GetBuffer(n, IID_PPV_ARGS(&renderTargets[n])));
#ifdef _DEBUG
                renderTargets[n]->SetName(L"SwapChain BackBuffer " + n);
#endif
                device->getDevice()->CreateRenderTargetView(renderTargets[n].Get(), &rtvDesc, rtvHandle);
                rtvHandle.Offset(1, rtvDescriptorSize);
            }

            currentFrameIndex = swapChain->GetCurrentBackBufferIndex();
        }
    }

    // void DXSwapChain::waitForLastPresentedFrame() {
    //     dxCheck(presentCommandQueue->Signal(device->getInFlightFence().Get(), lastPresentedFenceValue));
    //     if (device->getInFlightFence()->GetCompletedValue() < lastPresentedFenceValue) {
    //         dxCheck(device->getInFlightFence()->SetEventOnCompletion(
    //             lastPresentedFenceValue,
    //             device->getInFlightFenceEvent()
    //         ));
    //         WaitForSingleObjectEx(device->getInFlightFenceEvent(), INFINITE, FALSE);
    //     }
    // }

    void DXSwapChain::nextSwapChain() {
        currentFrameIndex = swapChain->GetCurrentBackBufferIndex();
        assert(currentFrameIndex < FRAMES_IN_FLIGHT);
    }

    bool DXSwapChain::acquire(const shared_ptr<Fence>& fence) {
        const auto dxFence = static_pointer_cast<DXFence>(fence);
        // If the next frame is not ready to be rendered yet, wait until it is ready.
        if (dxFence->getFence()->GetCompletedValue() < dxFence->getValue()) {
            dxCheck(dxFence->getFence()->SetEventOnCompletion(
                dxFence->getValue(),
                fenceEvent
            ));
            WaitForSingleObjectEx(fenceEvent, INFINITE, FALSE);
        }
        dxFence->increment();
        return true;
    }

    void DXSwapChain::present(const shared_ptr<FrameData>& frameData) {
        const auto data = static_pointer_cast<DXFrameData>(frameData);
        dxCheck(swapChain->Present(syncInterval, presentFlags));
    }
}
