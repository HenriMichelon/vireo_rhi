/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/backend/directx/Tools.h"
module vireo.directx.swapchains;

namespace vireo {

    DXSwapChain::DXSwapChain(
        const ComPtr<IDXGIFactory4>& factory,
        DXDevice& dxdevice,
        const ComPtr<ID3D12CommandQueue>& commandQueue,
        const uint32_t width,
        const uint32_t height,
        HWND hWnd) :
        device{dxdevice},
        presentCommandQueue{commandQueue},
        hWnd{hWnd} {
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
            hWnd,
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
            DieIfFailed(swapChain->GetBuffer(n, IID_PPV_ARGS(&renderTargets[n])));
#ifdef _DEBUG
            renderTargets[n]->SetName(L"SwapChain BackBuffer " + n);
#endif
            device.getDevice()->CreateRenderTargetView(renderTargets[n].Get(), &rtvDesc, rtvHandle);
            rtvHandle.Offset(1, rtvDescriptorSize);
        }
    }

    void DXSwapChain::nextSwapChain() {
        currentFrameIndex = swapChain->GetCurrentBackBufferIndex();
        assert(currentFrameIndex < FRAMES_IN_FLIGHT);
    }

    bool DXSwapChain::acquire(shared_ptr<FrameData>& frameData) {
        auto data = static_pointer_cast<DXFrameData>(frameData);
        const auto currentFenceValue = data->inFlightFenceValue;
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

    void DXSwapChain::present(shared_ptr<FrameData>& frameData) {
        auto data = static_pointer_cast<DXFrameData>(frameData);
        DieIfFailed(swapChain->Present(1, 0));
        data->inFlightFenceValue += 1;
    }
}
