module;
#include "DXLibraries.h"
import std;

export module dxvk.backend.directx;

import dxvk.backend;
import dxvk.backend.framedata;

export namespace dxvk::backend {

    class DXInstance : public Instance {
    public:
        DXInstance();
        auto getFactory() { return factory; }

    private:
        ComPtr<IDXGIFactory4> factory;
    };

    class DXPhysicalDevice : public PhysicalDevice {
    public:
        DXPhysicalDevice(ComPtr<IDXGIFactory4> factory);
        auto getHardwareAdapater() { return hardwareAdapter4; }

    private:
        ComPtr<IDXGIAdapter4> hardwareAdapter4;
    };

    class DXDevice : public Device {
    public:
        DXDevice(ComPtr<IDXGIAdapter4> hardwareAdapter4);
        auto getDevice() { return device; }

    private:
        ComPtr<ID3D12Device> device;
    };

    class DXCommandQueue : public CommandQueue{
    public:
        DXCommandQueue(ComPtr<ID3D12Device> device);
        auto getCommandQueue() { return commandQueue; }
    private:
        ComPtr<ID3D12CommandQueue> commandQueue;
    };

    class DXSwapChain : public SwapChain {
    public:
        DXSwapChain(
            ComPtr<IDXGIFactory4> factory,
            ComPtr<ID3D12Device> device,
            ComPtr<ID3D12CommandQueue> commandQueue,
            uint32_t with, uint32_t height);

        auto getSwapChain() { return swapChain; }

        auto getCurrentFrameIndex() const { return currentFrameIndex; }

        void nextSwapChain() override;

        void present(const FrameData& frameData) override;

    private:
        ComPtr<IDXGISwapChain3>      swapChain;
        ComPtr<ID3D12DescriptorHeap> rtvHeap;
        UINT                         rtvDescriptorSize;
        ComPtr<ID3D12Resource>       renderTargets[backend::SwapChain::FRAMES_IN_FLIGHT];
        UINT                         currentFrameIndex;
    };

    class DXRenderingBackEnd : public RenderingBackEnd {
    public:
        DXRenderingBackEnd(uint32_t width, uint32_t height);

        auto getDXInstance() const { return std::reinterpret_pointer_cast<DXInstance>(instance); }
        auto getDXPhysicalDevice() const { return std::reinterpret_pointer_cast<DXPhysicalDevice>(physicalDevice); }
        auto getDXDevice() const { return std::reinterpret_pointer_cast<DXDevice>(device); }
        auto getDXGraphicCommandQueue() const { return std::reinterpret_pointer_cast<DXCommandQueue>(graphicCommandQueue); }
        auto getDXSwapChain() const { return std::reinterpret_pointer_cast<DXSwapChain>(swapChain); }
    };

}