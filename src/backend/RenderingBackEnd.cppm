module;
#include "Libraries.h"
import std;

export module dxvk.backend;

export namespace dxvk::backend {

    class PhysicalDevice {
    public:
        virtual ~PhysicalDevice() = default;
    };

    class Instance {
    public:
        virtual ~Instance() = default;
    };

    class RenderingBackEnd
    {
    public:
        virtual ~RenderingBackEnd() = default;

        auto getInstance() { return instance; }
        auto getPhysicalDevice() { return physicalDevice; }

    protected:
        std::shared_ptr<Instance> instance;
        std::shared_ptr<PhysicalDevice> physicalDevice;

    };
}