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
import vireo.config;
import vireo.directx.devices;
import vireo.directx.framedata;

export namespace vireo {

    class DXSwapChain : public SwapChain {
    public:
        static constexpr auto RENDER_FORMAT{DXGI_FORMAT_R8G8B8A8_UNORM_SRGB};

        DXSwapChain(
            const ComPtr<IDXGIFactory4>& factory,
            const shared_ptr<DXDevice>& device,
            const ComPtr<ID3D12CommandQueue>& commandQueue,
            HWND hWnd, PresentMode vSyncMode);

        ~DXSwapChain() override;

        auto getSwapChain() { return swapChain; }

        auto getRenderTargets() const { return renderTargets; }

        auto getDescriptorSize() const {  return rtvDescriptorSize; }

        auto getHeap() const { return rtvHeap; }

        void nextSwapChain() override;

        bool acquire(const shared_ptr<FrameData>& frameData) override;

        void present(const shared_ptr<FrameData>& frameData) override;

        void recreate() override;

        void waitForLastPresentedFrame();

    private:
        const shared_ptr<DXDevice>   device;
        const ComPtr<IDXGIFactory4>  factory;
        ComPtr<IDXGISwapChain3>      swapChain;
        ComPtr<ID3D12CommandQueue>   presentCommandQueue;
        ComPtr<ID3D12Resource>       renderTargets[FRAMES_IN_FLIGHT];
        ComPtr<ID3D12DescriptorHeap> rtvHeap;
        UINT                         rtvDescriptorSize{0};
        HWND                         hWnd;
        const UINT                   syncInterval;
        const UINT                   presentFlags;
        UINT64                       lastPresentedFenceValue{0};

        void create();
    };

}