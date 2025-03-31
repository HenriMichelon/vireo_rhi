module;
#include "DXLibraries.h"
import std;

export module dxvk.backend.directx;

import dxvk.backend;

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

    class DXRenderingBackEnd : public RenderingBackEnd {
    public:
        DXRenderingBackEnd();

        auto getDXInstance() { return std::reinterpret_pointer_cast<DXInstance>(instance); }
        auto getDXPhysicalDevice() { return std::reinterpret_pointer_cast<DXPhysicalDevice>(physicalDevice); }
    };

}