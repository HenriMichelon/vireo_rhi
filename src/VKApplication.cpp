// https://github.com/microsoft/DirectX-Graphics-Samples/tree/master/Samples/Desktop/D3D12HelloWorld/src/HelloTriangle
module;
#include "VKLibraries.h"
#include "VKTools.h"

module dxvk.app.vulkan;

import dxvk.app.win32;

namespace dxvk {

    VKApplication::VKApplication(UINT width, UINT height, std::wstring name) : BaseApplication(width, height, name)
    {
    }

    void VKApplication::OnInit() {
        LoadPipeline();
    }

    void VKApplication::OnUpdate() {
        const float translationSpeed = 0.005f;
        const float offsetBounds = 1.25f;

        m_constantBufferData.offset.x += translationSpeed;
        if (m_constantBufferData.offset.x > offsetBounds)
        {
            m_constantBufferData.offset.x = -offsetBounds;
        }
    }

    void VKApplication::OnRender() {

    }

    void VKApplication::OnDestroy() {

    }

    void VKApplication::LoadPipeline() {

    }
}