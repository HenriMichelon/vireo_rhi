module;
#include "DXLibraries.h"
#include "DXTools.h"
import std;

module dxvk.backend.directx;

import dxvk.app.win32;

namespace dxvk::backend {

    DXRenderingBackEnd::DXRenderingBackEnd(uint32_t width, uint32_t height) {
        instance = std::make_shared<DXInstance>();
        physicalDevice = std::make_shared<DXPhysicalDevice>(getDXInstance()->getFactory());
        device = std::make_shared<DXDevice>(getDXPhysicalDevice()->getHardwareAdapater());
        graphicCommandQueue = std::make_shared<DXCommandQueue>(getDXDevice()->getDevice());
        swapChain = std::make_shared<DXSwapChain>(
            getDXInstance()->getFactory(),
            getDXGraphicCommandQueue()->getCommandQueue(),
            width, height);
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
        // This sample does not support fullscreen transitions.
        ThrowIfFailed(factory->MakeWindowAssociation(Win32Application::getHwnd(), DXGI_MWA_NO_ALT_ENTER));
    }

    DXPhysicalDevice::DXPhysicalDevice(ComPtr<IDXGIFactory4> factory) {
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
                dxgiAdapterDesc1.DedicatedVideoMemory > maxDedicatedVideoMemory )
            {
                maxDedicatedVideoMemory = dxgiAdapterDesc1.DedicatedVideoMemory;
                ThrowIfFailed(hardwareAdapter.As(&hardwareAdapter4));
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

    DXDevice::DXDevice(ComPtr<IDXGIAdapter4> hardwareAdapter4) {
        ThrowIfFailed(
            D3D12CreateDevice(
                  hardwareAdapter4.Get(),
                  D3D_FEATURE_LEVEL_11_0,
                  IID_PPV_ARGS(&device)
          ));
    }

    DXCommandQueue::DXCommandQueue(ComPtr<ID3D12Device> device) {
        // Describe and create the command queue.
        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        ThrowIfFailed(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue)));
    }

    DXSwapChain::DXSwapChain(
        ComPtr<IDXGIFactory4> factory,
        ComPtr<ID3D12CommandQueue> commandQueue,
        uint32_t width,
        uint32_t height) {
        extent = { width, height };

        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.BufferCount = SwapChain::FRAMES_IN_FLIGHT;
        swapChainDesc.Width = width;
        swapChainDesc.Height = height;
        swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapChainDesc.SampleDesc.Count = 1;

        ComPtr<IDXGISwapChain1> swapChain1;
        ThrowIfFailed(factory->CreateSwapChainForHwnd(
            commandQueue.Get(),        // Swap chain needs the queue so that it can force a flush on it.
            Win32Application::getHwnd(),
            &swapChainDesc,
            nullptr,
            nullptr,
            &swapChain1
            ));
        ThrowIfFailed(swapChain1.As(&swapChain));
        nextSwapChain();
    }

    void DXSwapChain::nextSwapChain() {
        currentFrameIndex = swapChain->GetCurrentBackBufferIndex();
    }


}