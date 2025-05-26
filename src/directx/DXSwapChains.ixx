/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/backend/directx/Libraries.h"
export module vireo.directx.swapchains;

import vireo;
import vireo.directx.commands;
import vireo.directx.devices;

export namespace vireo {

    class DXSwapChain : public SwapChain {
    public:
        DXSwapChain(
            const ComPtr<IDXGIFactory4>& factory,
            const std::shared_ptr<DXDevice>& device,
            const ComPtr<ID3D12CommandQueue>& commandQueue,
            ImageFormat format,
            HWND hWnd, PresentMode vSyncMode, uint32_t framesInFlight);

        ~DXSwapChain() override;

        auto getSwapChain() { return swapChain; }

        auto getRenderTargets() const { return renderTargets; }

        auto getDescriptorSize() const {  return rtvDescriptorSize; }

        auto getHeap() const { return rtvHeap; }

        auto getFence() const { return fence; }

        auto getFenceValue() const { return fenceValue; }

        void nextFrameIndex() override;

        bool acquire(const std::shared_ptr<Fence>& fence) override;

        void present() override;

        void recreate() override;

        void waitIdle() override;

    private:
        const std::shared_ptr<DXDevice>device;
        const ComPtr<IDXGIFactory4>    factory;
        ComPtr<IDXGISwapChain3>        swapChain;
        // Submission queue to present the frame buffers on the rendering surface
        ComPtr<ID3D12CommandQueue>     presentCommandQueue;
        // Frame buffers
        std::vector<ComPtr<ID3D12Resource>> renderTargets;
        // D3D12_DESCRIPTOR_HEAP_TYPE_RTV heap for the frame buffers
        ComPtr<ID3D12DescriptorHeap>   rtvHeap;
        UINT                           rtvDescriptorSize{0};
        HWND                           hWnd;
        const UINT                     syncInterval;
        const UINT                     presentFlags;
        // Fences used to wait for the previous presentation to be finished before acquiring the frame buffer
        ComPtr<ID3D12Fence>            fence;
        HANDLE                         fenceEvent;
        UINT64                         fenceValue{0};

        void create();
    };

}