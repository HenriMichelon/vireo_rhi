module;
#include "Libraries.h"
import std;

export module dxvk.backend;

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

    class RenderingBackEnd
    {
    public:
        virtual ~RenderingBackEnd() = default;

        auto& getInstance() const { return instance; }
        auto& getPhysicalDevice() const { return physicalDevice; }
        auto& getDevice() const { return device; }
        auto& getGraphicCommandQueue() const { return graphicCommandQueue; }

    protected:
        std::shared_ptr<Instance>        instance;
        std::shared_ptr<PhysicalDevice>  physicalDevice;
        std::shared_ptr<Device>          device;
        std::shared_ptr<CommandQueue>    graphicCommandQueue;

    };
}