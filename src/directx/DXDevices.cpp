/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/backend/directx/Libraries.h"
module vireo.directx.devices;

import std;
import vireo.directx.tools;

namespace vireo {

    DXInstance::DXInstance() {
        UINT dxgiFactoryFlags = 0;
#if defined(_DEBUG)
        {
            ComPtr<ID3D12Debug> debugController;
            if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
                debugController->EnableDebugLayer();
                dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
            }
        }
#endif
        dxCheck(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));
    }

    DXPhysicalDevice::DXPhysicalDevice(const ComPtr<IDXGIFactory4>& factory) {
        ComPtr<IDXGIAdapter1> hardwareAdapter;

        SIZE_T maxDedicatedVideoMemory = 0;
        for (UINT i = 0; factory->EnumAdapters1(i, &hardwareAdapter) != DXGI_ERROR_NOT_FOUND; ++i) {
            DXGI_ADAPTER_DESC1 dxgiAdapterDesc1;
            hardwareAdapter->GetDesc1(&dxgiAdapterDesc1);

            // Check to see if the adapter can create a D3D12 device without actually
            // creating it. The adapter with the largest dedicated video memory
            // is favored.
            if ((dxgiAdapterDesc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0 &&
                SUCCEEDED(D3D12CreateDevice(hardwareAdapter.Get(),
                    D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr)) &&
                dxgiAdapterDesc1.DedicatedVideoMemory > maxDedicatedVideoMemory) {
                maxDedicatedVideoMemory = dxgiAdapterDesc1.DedicatedVideoMemory;
                dxCheck(hardwareAdapter.As(&hardwareAdapter4));
            }
        }
        if (hardwareAdapter == nullptr) {
            throw Exception("Failed to find GPUs with DirectX 12 support");
        }
    }

    const PhysicalDeviceDesc DXPhysicalDevice::getDescription() const {
        DXGI_ADAPTER_DESC3 desc;
        dxCheck(hardwareAdapter4->GetDesc3(&desc));
        return {
            desc.Description,
            desc.DedicatedVideoMemory,
            desc.DedicatedSystemMemory,
            desc.SharedSystemMemory,
        };
    }

    DXDevice::DXDevice(const ComPtr<IDXGIAdapter4>& hardwareAdapter4) {
        dxCheck(
            D3D12CreateDevice(
                hardwareAdapter4.Get(),
                D3D_FEATURE_LEVEL_11_0,
                IID_PPV_ARGS(&device)
                ));
#if defined(_DEBUG)
        ComPtr<ID3D12InfoQueue> infoQueue;
        if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
            infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
            infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
            infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, FALSE);
        }
#endif
    }

}
