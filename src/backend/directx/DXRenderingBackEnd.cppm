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

        void waitIdle() override {};

        auto getDevice() { return device; }

        auto getInFlightFence() { return inFlightFence; }

        auto getInFlightFenceEvent() const { return inFlightFenceEvent;}

        std::shared_ptr<CommandAllocator> createCommandAllocator(CommandAllocator::Type type) const override;

    private:
        ComPtr<ID3D12Device> device;
        ComPtr<ID3D12Fence>  inFlightFence;
        HANDLE               inFlightFenceEvent;
    };

    class DXSubmitQueue : public SubmitQueue{
    public:
        DXSubmitQueue(ComPtr<ID3D12Device> device);

        auto getCommandQueue() { return commandQueue; }

        void submit(const FrameData& frameData, std::vector<std::shared_ptr<CommandList>> commandLists) override { }

    private:
        ComPtr<ID3D12CommandQueue> commandQueue;
    };

    class DXCommandAllocator : public CommandAllocator {
    public:
        DXCommandAllocator(ComPtr<ID3D12Device> device);

        std::shared_ptr<CommandList> createCommandList() const override;

    private:
        ComPtr<ID3D12Device>           device;
        ComPtr<ID3D12CommandAllocator> commandAllocator;
    };

    class DXCommandList : public CommandList {
    public:
        DXCommandList(ComPtr<ID3D12Device> device, ComPtr<ID3D12CommandAllocator> commandAllocator);

        void begin() override;
        void end() override;

        ~DXCommandList() override = default;
    private:
        ComPtr<ID3D12GraphicsCommandList> commandList;
        ComPtr<ID3D12CommandAllocator>    commandAllocator;
    };

    class DXSwapChain : public SwapChain {
    public:
        DXSwapChain(
            ComPtr<IDXGIFactory4> factory,
            DXDevice& device,
            ComPtr<ID3D12CommandQueue> commandQueue,
            uint32_t with, uint32_t height);

        auto getSwapChain() { return swapChain; }

        void nextSwapChain() override;

        void acquire(FrameData& frameData) override;

        void present(FrameData& frameData) override;

    private:
        DXDevice&                    device;
        ComPtr<IDXGISwapChain3>      swapChain;
        ComPtr<ID3D12CommandQueue>   presentCommandQueue;
        ComPtr<ID3D12Resource>       renderTargets[FRAMES_IN_FLIGHT];
        ComPtr<ID3D12DescriptorHeap> rtvHeap;
        UINT                         rtvDescriptorSize{0};
    };

    class DXRenderingBackEnd : public RenderingBackEnd {
    public:
        DXRenderingBackEnd(uint32_t width, uint32_t height);

        std::shared_ptr<FrameData> createFrameData(uint32_t frameIndex) override;

        void destroyFrameData(std::shared_ptr<FrameData> frameData) override {}

        auto getDXInstance() const { return std::reinterpret_pointer_cast<DXInstance>(instance); }

        auto getDXPhysicalDevice() const { return std::reinterpret_pointer_cast<DXPhysicalDevice>(physicalDevice); }

        auto getDXDevice() const { return std::reinterpret_pointer_cast<DXDevice>(device); }

        auto getDXGraphicCommandQueue() const { return std::reinterpret_pointer_cast<DXSubmitQueue>(graphicCommandQueue); }

        auto getDXPresentCommandQueue() const { return std::reinterpret_pointer_cast<DXSubmitQueue>(presentCommandQueue); }

        auto getDXTransferCommandQueue() const { return std::reinterpret_pointer_cast<DXSubmitQueue>(transferCommandQueue); }

        auto getDXSwapChain() const { return std::reinterpret_pointer_cast<DXSwapChain>(swapChain); }
    };

}