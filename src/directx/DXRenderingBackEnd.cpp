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
        // const HANDLE hMap = OpenFileMapping(FILE_MAP_READ, FALSE, L"RTSSSharedMemoryV2");
        // if (hMap) {
        //     MessageBox(
        //         nullptr,
        //         L"RivaTuner Statistic Server is incompatible with the DirectX 12 backend, close it or use the Vulkan backend",
        //         nullptr,
        //         MB_OK);
        //     CloseHandle(hMap);
        //     die("RTSS conflict");
        // }

        RECT windowRect{};
        if (GetClientRect(hWnd, &windowRect) == 0) {
            die("Error getting window rect");
        }

        instance = make_shared<DXInstance>(hWnd);
        physicalDevice = make_shared<DXPhysicalDevice>(getDXInstance()->getFactory());
        device = make_shared<DXDevice>(getDXPhysicalDevice()->getHardwareAdapter());
        computeCommandQueue = make_shared<DXSubmitQueue>(getDXDevice()->getDevice(), CommandType::COMPUTE);
        graphicCommandQueue = make_shared<DXSubmitQueue>(getDXDevice()->getDevice(), CommandType::GRAPHIC);
        transferCommandQueue = make_shared<DXSubmitQueue>(getDXDevice()->getDevice(), CommandType::GRAPHIC);
        swapChain = make_shared<DXSwapChain>(
            getDXInstance()->getFactory(),
            getDXDevice(),
            getDXGraphicCommandQueue()->getCommandQueue(),
            windowRect.right - windowRect.left,
            windowRect.bottom - windowRect.top,
            hWnd,
            configuration.vSyncMode);
    }

    shared_ptr<FrameData> DXRenderingBackEnd::createFrameData(const uint32_t frameIndex) {
        return make_shared<DXFrameData>();
    }

    shared_ptr<PipelineResources> DXRenderingBackEnd::createPipelineResources(
        const vector<shared_ptr<DescriptorLayout>>& descriptorLayouts,
        const PushConstantsDesc& pushConstant,
        const wstring& name ) const {
        return make_shared<DXPipelineResources>(getDXDevice()->getDevice(), descriptorLayouts, pushConstant, name);
    }

    shared_ptr<GraphicPipeline> DXRenderingBackEnd::createGraphicPipeline(
        const shared_ptr<PipelineResources>& pipelineResources,
        const shared_ptr<const VertexInputLayout>& vertexInputLayout,
        const shared_ptr<const ShaderModule>& vertexShader,
        const shared_ptr<const ShaderModule>& fragmentShader,
        const GraphicPipeline::Configuration& configuration,
        const wstring& name) const {
        return make_shared<DXGraphicPipeline>(
            getDXDevice()->getDevice(),
            pipelineResources,
            vertexInputLayout,
            vertexShader,
            fragmentShader,
            configuration,
            name);
    }

    shared_ptr<ComputePipeline> DXRenderingBackEnd::createComputePipeline(
        const shared_ptr<PipelineResources>& pipelineResources,
        const shared_ptr<const ShaderModule>& shader,
        const wstring& name) const {
            return make_shared<DXComputePipeline>(
                getDXDevice()->getDevice(),
                pipelineResources,
                shader,
                name);
    }

    shared_ptr<VertexInputLayout> DXRenderingBackEnd::createVertexLayout(
        size_t,
        const vector<VertexAttributeDesc>& attributesDescriptions) const {
        return make_shared<DXVertexInputLayout>(attributesDescriptions);
    }

    shared_ptr<ShaderModule> DXRenderingBackEnd::createShaderModule(const string& fileName) const {
        return make_shared<DXShaderModule>(fileName);
    }

    shared_ptr<Buffer> DXRenderingBackEnd::createBuffer(
        const BufferType type,
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
        return make_shared<DXImage>(getDXDevice()->getDevice(), format, width, height, name, false, false);
    }

    shared_ptr<Image> DXRenderingBackEnd::createReadWriteImage(
        ImageFormat format,
        uint32_t width,
        uint32_t height,
        const wstring& name) const {
            return make_shared<DXImage>(getDXDevice()->getDevice(), format, width, height, name, true, false);
    }

    shared_ptr<RenderTarget> DXRenderingBackEnd::createRenderTarget(
           const ImageFormat format,
           const uint32_t width,
           const uint32_t height,
           const wstring& name) const {
        return make_shared<DXRenderTarget>(
            getDXDevice()->getDevice(),
            make_shared<DXImage>(getDXDevice()->getDevice(), format, width, height, name, false, true));
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

    shared_ptr<CommandAllocator> DXRenderingBackEnd::createCommandAllocator(const CommandType type) const {
        return make_shared<DXCommandAllocator>(getDXDevice()->getDevice(), type);
    }

    void DXRenderingBackEnd::waitIdle() {
        graphicCommandQueue->waitIdle();
    }

}
