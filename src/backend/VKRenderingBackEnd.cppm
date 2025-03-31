module;
#include "VKLibraries.h"
import std;

export module dxvk.backend.vulkan;

import dxvk.backend;

export namespace dxvk::backend {

    class VKInstance : public Instance {
    public:
        VKInstance();
        ~VKInstance() override;
        auto getInstance() { return instance; }
    private:
        VkInstance instance{VK_NULL_HANDLE};
    };

    class VKRenderingBackEnd : public RenderingBackEnd {
    public:
        VKRenderingBackEnd();
    };

}