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

    class DXRenderingBackEnd : public RenderingBackEnd {
    public:
        DXRenderingBackEnd();
    };

}