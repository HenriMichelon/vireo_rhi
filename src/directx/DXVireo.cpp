/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/backend/directx/Libraries.h"
module vireo.directx;

import vireo.directx.descriptors;
import vireo.directx.framedata;
import vireo.directx.pipelines;
import vireo.directx.resources;

namespace vireo {

    DXVireo::DXVireo(const Configuration& configuration):
        Vireo{configuration},
        hWnd{static_cast<HWND>(configuration.windowHandle)} {
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
            hWnd,
            configuration.presentMode);
    }

    shared_ptr<FrameData> DXVireo::createFrameData(const uint32_t frameIndex) {
        return make_shared<DXFrameData>();
    }

    shared_ptr<PipelineResources> DXVireo::createPipelineResources(
        const vector<shared_ptr<DescriptorLayout>>& descriptorLayouts,
        const PushConstantsDesc& pushConstant,
        const wstring& name ) const {
        return make_shared<DXPipelineResources>(getDXDevice()->getDevice(), descriptorLayouts, pushConstant, name);
    }

    shared_ptr<GraphicPipeline> DXVireo::createGraphicPipeline(
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

    shared_ptr<ComputePipeline> DXVireo::createComputePipeline(
        const shared_ptr<PipelineResources>& pipelineResources,
        const shared_ptr<const ShaderModule>& shader,
        const wstring& name) const {
            return make_shared<DXComputePipeline>(
                getDXDevice()->getDevice(),
                pipelineResources,
                shader,
                name);
    }

    shared_ptr<VertexInputLayout> DXVireo::createVertexLayout(
        size_t,
        const vector<VertexAttributeDesc>& attributesDescriptions) const {
        return make_shared<DXVertexInputLayout>(attributesDescriptions);
    }

    shared_ptr<ShaderModule> DXVireo::createShaderModule(const string& fileName) const {
        return make_shared<DXShaderModule>(fileName);
    }

    shared_ptr<Buffer> DXVireo::createBuffer(
        const BufferType type,
        const size_t size,
        const size_t count,
        const size_t alignment,
        const wstring& name) const {
        return make_shared<DXBuffer>(getDXDevice()->getDevice(), type, size, count, alignment, name);
    }

    shared_ptr<Image> DXVireo::createImage(
        ImageFormat format,
        uint32_t width,
        uint32_t height,
        const wstring& name) const {
        return make_shared<DXImage>(getDXDevice()->getDevice(), format, width, height, name, false, false);
    }

    shared_ptr<Image> DXVireo::createReadWriteImage(
        ImageFormat format,
        uint32_t width,
        uint32_t height,
        const wstring& name) const {
            return make_shared<DXImage>(getDXDevice()->getDevice(), format, width, height, name, true, false);
    }

    shared_ptr<RenderTarget> DXVireo::createRenderTarget(
           const ImageFormat format,
           const uint32_t width,
           const uint32_t height,
           const wstring& name) const {
        return make_shared<DXRenderTarget>(
            getDXDevice()->getDevice(),
            make_shared<DXImage>(getDXDevice()->getDevice(), format, width, height, name, false, true));
    }

    shared_ptr<DescriptorLayout> DXVireo::createDescriptorLayout(
        const wstring& name) {
        return make_shared<DXDescriptorLayout>(false);
    }

    shared_ptr<DescriptorLayout> DXVireo::createSamplerDescriptorLayout(
        const wstring& name) {
        return make_shared<DXDescriptorLayout>(true);
    }

    shared_ptr<DescriptorSet> DXVireo::createDescriptorSet(
        const shared_ptr<const DescriptorLayout>& layout,
        const wstring& name) {
        return make_shared<DXDescriptorSet>(layout, getDXDevice()->getDevice(), name);
    }

    shared_ptr<Sampler> DXVireo::createSampler(
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

    shared_ptr<CommandAllocator> DXVireo::createCommandAllocator(const CommandType type) const {
        return make_shared<DXCommandAllocator>(getDXDevice()->getDevice(), type);
    }

    DXVireo::~DXVireo() {
        DXVireo::waitIdle();
    }

    void DXVireo::waitIdle() {
        getDXSwapChain()->waitForLastPresentedFrame();
        graphicCommandQueue->waitIdle();
        computeCommandQueue->waitIdle();
        transferCommandQueue->waitIdle();
    }

}
