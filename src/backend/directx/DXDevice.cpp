/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/backend/directx/Tools.h"
module vireo.backend.directx.device;

import vireo.app.win32;

namespace vireo::backend {

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
        DieIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));
        // This sample does not support fullscreen transitions.
        DieIfFailed(factory->MakeWindowAssociation(Win32Application::getHwnd(), DXGI_MWA_NO_ALT_ENTER));
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
                DieIfFailed(hardwareAdapter.As(&hardwareAdapter4));
            }
        }
        DXGI_ADAPTER_DESC3 desc;
        const HRESULT hr = hardwareAdapter4->GetDesc3(&desc);
        if (SUCCEEDED(hr)) {
            std::wstring adapterName = desc.Description;
            std::wcout << L"Display Adapter Name: " << adapterName << std::endl;
        } else {
            std::cerr << "Failed to get adapter description." << std::endl;
        }
    }

    DXDevice::DXDevice(const ComPtr<IDXGIAdapter4>& hardwareAdapter4) {
        DieIfFailed(
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
            infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);
        }
#endif
        DieIfFailed(device->CreateFence(
            0,
            D3D12_FENCE_FLAG_NONE,
            IID_PPV_ARGS(&inFlightFence)));
        inFlightFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (inFlightFenceEvent == nullptr) {
            DieIfFailed(HRESULT_FROM_WIN32(GetLastError()));
        }
    }

    DXDevice::~DXDevice() {
        CloseHandle(inFlightFenceEvent);
    }

}
