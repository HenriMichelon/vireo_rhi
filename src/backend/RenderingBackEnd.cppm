module;
#include "Libraries.h"
import std;

export module dxvk.backend;

import dxvk.backend.framedata;

export namespace dxvk::backend {

    class Instance {
    public:
        virtual ~Instance() = default;
    };

    class PhysicalDevice {
    public:
        virtual ~PhysicalDevice() = default;
    };

    class Device {
    public:
        virtual ~Device() = default;
    };

    class CommandQueue {
    public:
        virtual ~CommandQueue() = default;
    };

    class SwapChain {
    public:
        static constexpr uint32_t FRAMES_IN_FLIGHT = 2;

        struct Extent {
            uint32_t width;
            uint32_t height;
        };

        virtual ~SwapChain() = default;

        auto& getExtent() const { return extent; }

        virtual void nextSwapChain() = 0;

        virtual void present(const FrameData& frameData) = 0;

        virtual void prepare(FrameData& frameData) {}

    protected:
        Extent extent{};
    };

    class RenderingBackEnd
    {
    public:
        virtual ~RenderingBackEnd() = default;

        auto& getInstance() const { return instance; }
        auto& getPhysicalDevice() const { return physicalDevice; }
        auto& getDevice() const { return device; }
        auto& getGraphicCommandQueue() const { return graphicCommandQueue; }
        auto& getSwapChain() const { return swapChain; }

    protected:
        std::shared_ptr<Instance>        instance;
        std::shared_ptr<PhysicalDevice>  physicalDevice;
        std::shared_ptr<Device>          device;
        std::shared_ptr<CommandQueue>    graphicCommandQueue;
        std::shared_ptr<SwapChain>       swapChain;

    };
}