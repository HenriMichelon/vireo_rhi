module;
#include "DXLibraries.h"
#include "DXTools.h"
import std;

module dxvk.backend.directx;

namespace dxvk::backend {

    DXRenderingBackEnd::DXRenderingBackEnd() {
        instance = std::make_shared<DXInstance>();
    }

    DXInstance::DXInstance() {
        UINT dxgiFactoryFlags = 0;
#if defined(_DEBUG)
        {
            ComPtr<ID3D12Debug> debugController;
            if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
            {
                debugController->EnableDebugLayer();
                dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
            }
        }
#endif
        ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));

    }


}