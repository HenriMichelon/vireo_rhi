/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/backend/directx/Tools.h"
module vireo.directx;

import vireo.directx.descriptors;
import vireo.directx.framedata;
import vireo.directx.pipelines;
import vireo.directx.resources;

namespace vireo {

    DXRenderingBackEnd::DXRenderingBackEnd(const Configuration& configuration):
        RenderingBackEnd{configuration},
        hWnd{static_cast<HWND>(configuration.windowHandle)} {
        // Detect RivaTuner which cause problem by incorrectly hooking IDXGISwapChain::Present
        const HANDLE hMap = OpenFileMapping(FILE_MAP_READ, FALSE, L"RTSSSharedMemoryV2");
        if (hMap) {
            MessageBox(
                nullptr,
                L"RivaTuner Statistic Server is incompatible with the DirectX 12 backend, close it or use the Vulkan backend",
                nullptr,
                MB_OK);
            CloseHandle(hMap);
            die("RTSS conflict");
        }

        RECT windowRect{};
        if (GetClientRect(hWnd, &windowRect) == 0) {
            die("Error getting window rect");
        }
        auto width = windowRect.right - windowRect.left;
        auto height = windowRect.bottom - windowRect.top;
        viewport = CD3DX12_VIEWPORT{0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height)},
        scissorRect = CD3DX12_RECT{0, 0, static_cast<LONG>(width), static_cast<LONG>(height)};

        instance = make_shared<DXInstance>(hWnd);
        physicalDevice = make_shared<DXPhysicalDevice>(getDXInstance()->getFactory());
        device = make_shared<DXDevice>(getDXPhysicalDevice()->getHardwareAdapter());
        graphicCommandQueue = make_shared<DXSubmitQueue>(getDXDevice()->getDevice(), CommandList::GRAPHIC);
        transferCommandQueue = make_shared<DXSubmitQueue>(getDXDevice()->getDevice(), CommandList::GRAPHIC);
        swapChain = make_shared<DXSwapChain>(
            getDXInstance()->getFactory(),
            *getDXDevice(),
            getDXGraphicCommandQueue()->getCommandQueue(),
            width,
            height,
            hWnd,
            configuration.vSyncMode);
    }

    shared_ptr<FrameData> DXRenderingBackEnd::createFrameData(const uint32_t frameIndex) {
        return make_shared<DXFrameData>();
    }

    shared_ptr<PipelineResources> DXRenderingBackEnd::createPipelineResources(
        const vector<shared_ptr<DescriptorLayout>>& descriptorLayouts,
        const wstring& name ) const {
        return make_shared<DXPipelineResources>(getDXDevice()->getDevice(), descriptorLayouts, name);
    }

    shared_ptr<Pipeline> DXRenderingBackEnd::createPipeline(
        const shared_ptr<PipelineResources>& pipelineResources,
        const shared_ptr<const VertexInputLayout>& vertexInputLayout,
        const shared_ptr<const ShaderModule>& vertexShader,
        const shared_ptr<const ShaderModule>& fragmentShader,
        const wstring& name) const {
        return make_shared<DXPipeline>(
            getDXDevice()->getDevice(),
            pipelineResources,
            vertexInputLayout,
            vertexShader,
            fragmentShader,
            name);
    }

    shared_ptr<VertexInputLayout> DXRenderingBackEnd::createVertexLayout(
        size_t,
        const vector<VertexInputLayout::AttributeDescription>& attributesDescriptions) const {
        return make_shared<DXVertexInputLayout>(attributesDescriptions);
    }

    shared_ptr<ShaderModule> DXRenderingBackEnd::createShaderModule(const string& fileName) const {
        return make_shared<DXShaderModule>(fileName);
    }

    shared_ptr<Buffer> DXRenderingBackEnd::createBuffer(
        const Buffer::Type type,
        const size_t size,
        const size_t count,
        const size_t alignment,
        const wstring& name) const {
        return make_shared<DXBuffer>(getDXDevice()->getDevice(), type, size, count, alignment, name);
    }

    shared_ptr<Image> DXRenderingBackEnd::createImage(
        ImageFormat format,
        uint32_t width,
        uint32_t height,
        const wstring& name) const {
        return make_shared<DXImage>(getDXDevice()->getDevice(), format, width, height, name);
    }

    shared_ptr<DescriptorLayout> DXRenderingBackEnd::createDescriptorLayout(
        const wstring& name) {
        return make_shared<DXDescriptorLayout>(false);
    }

    shared_ptr<DescriptorLayout> DXRenderingBackEnd::createSamplerDescriptorLayout(
        const wstring& name) {
        return make_shared<DXDescriptorLayout>(true);
    }

    shared_ptr<DescriptorSet> DXRenderingBackEnd::createDescriptorSet(
        const shared_ptr<const DescriptorLayout>& layout,
        const wstring& name) {
        return make_shared<DXDescriptorSet>(layout, getDXDevice()->getDevice(), name);
    }

    shared_ptr<Sampler> DXRenderingBackEnd::createSampler(
           Filter minFilter,
           Filter magFilter,
           AddressMode addressModeU,
           AddressMode addressModeV,
           AddressMode addressModeW,
           float minLod,
           float maxLod,
           bool anisotropyEnable,
           MipMapMode mipMapMode) const {
        return make_shared<DXSampler>(
            minFilter, magFilter, addressModeU, addressModeV, addressModeW,
            minLod, maxLod, anisotropyEnable, mipMapMode);
    }

    void DXRenderingBackEnd::beginRendering(const shared_ptr<FrameData>& data,
                                            const shared_ptr<const CommandList>& commandList) {
        const auto dxCommandList = static_pointer_cast<const DXCommandList>(commandList)->getCommandList();
        const auto dxSwapChain = getDXSwapChain();
        const auto frameIndex = swapChain->getCurrentFrameIndex();

        dxCommandList->RSSetViewports(1, &viewport);
        dxCommandList->RSSetScissorRects(1, &scissorRect);

        {
            const auto swapChainBarrier = CD3DX12_RESOURCE_BARRIER::Transition(dxSwapChain->getRenderTargets()[frameIndex].Get(),
                                                                         D3D12_RESOURCE_STATE_PRESENT,
                                                                         D3D12_RESOURCE_STATE_RENDER_TARGET);
            dxCommandList->ResourceBarrier(1, &swapChainBarrier);
        }

        {
            const CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(
                dxSwapChain->getHeap()->GetCPUDescriptorHandleForHeapStart(),
                frameIndex,
                dxSwapChain->getDescriptorSize());
            dxCommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

            const float dxClearColor[] = {clearColor[0], clearColor[1], clearColor[2], clearColor[3]};
            dxCommandList->ClearRenderTargetView(
                rtvHandle,
                dxClearColor,
                0,
                nullptr);
        }

        dxCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    }

    void DXRenderingBackEnd::endRendering(const shared_ptr<const CommandList>& commandList) {
        const auto dxCommandList = static_pointer_cast<const DXCommandList>(commandList)->getCommandList();
        const auto dxSwapChain = getDXSwapChain();
        const auto frameIndex = swapChain->getCurrentFrameIndex();

        const auto swapChainBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
            dxSwapChain->getRenderTargets()[frameIndex].Get(),
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            D3D12_RESOURCE_STATE_PRESENT);
        dxCommandList->ResourceBarrier(1, &swapChainBarrier);
    }

    shared_ptr<CommandAllocator> DXRenderingBackEnd::createCommandAllocator(const CommandList::Type type) const {
        return make_shared<DXCommandAllocator>(getDXDevice()->getDevice(), type);
    }

    void DXRenderingBackEnd::waitIdle() {
        graphicCommandQueue->waitIdle();
    }

}
