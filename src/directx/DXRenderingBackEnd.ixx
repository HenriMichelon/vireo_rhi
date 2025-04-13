/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/backend/directx/Libraries.h"
export module vireo.directx;

import vireo;
import vireo.config;
import vireo.directx.commands;
import vireo.directx.devices;
import vireo.directx.swapchains;

export namespace vireo {

    class DXRenderingBackEnd : public RenderingBackEnd {
    public:
        DXRenderingBackEnd(const Configuration& configuration);

        void waitIdle() override;

        shared_ptr<CommandAllocator> createCommandAllocator(CommandList::Type type) const override;

        shared_ptr<FrameData> createFrameData(
            uint32_t frameIndex,
            const vector<shared_ptr<DescriptorSet>>& descriptorSet) override;

        shared_ptr<VertexInputLayout> createVertexLayout(
            size_t size,
            const vector<VertexInputLayout::AttributeDescription>& attributesDescriptions) const override;

        shared_ptr<ShaderModule> createShaderModule(const string& fileName) const override;

        shared_ptr<PipelineResources> createPipelineResources(
            const vector<shared_ptr<DescriptorLayout>>& descriptorLayouts,
            const wstring& name = L"PipelineResource") const override;

        shared_ptr<Pipeline> createPipeline(
            PipelineResources& pipelineResources,
            VertexInputLayout& vertexInputLayout,
            ShaderModule& vertexShader,
            ShaderModule& fragmentShader,
            const wstring& name = L"Pipeline") const override;

        shared_ptr<Buffer> createBuffer(
            Buffer::Type type,
            size_t size,
            size_t count = 1,
            size_t alignment = 1,
            const wstring& name = L"Buffer") const override;

        shared_ptr<Image> createImage(
            ImageFormat format,
            uint32_t width,
            uint32_t height,
            const wstring& name = L"Image") const override;

        shared_ptr<DescriptorLayout> createDescriptorLayout(
            const wstring& name) override;

        shared_ptr<DescriptorLayout> createSamplerDescriptorLayout(
            const wstring& name = L"createSamplerDescriptorLayout") override;

        shared_ptr<DescriptorSet> createDescriptorSet(
            shared_ptr<DescriptorLayout>& layout,
            const wstring& name) override;

        shared_ptr<Sampler> createSampler(
               Filter minFilter,
               Filter magFilter,
               AddressMode addressModeU,
               AddressMode addressModeV,
               AddressMode addressModeW,
               float minLod = 0.0f,
               float maxLod = 1.0f,
               bool anisotropyEnable = true,
               MipMapMode mipMapMode = MipMapMode::LINEAR) const override;

        void beginRendering(
            shared_ptr<FrameData>& frameData,
            shared_ptr<PipelineResources>& pipelineResources,
            shared_ptr<Pipeline>& pipeline,
            shared_ptr<CommandList>& commandList) override;

        void endRendering(shared_ptr<CommandList>& commandList) override;

        auto getDXInstance() const { return reinterpret_pointer_cast<DXInstance>(instance); }

        auto getDXPhysicalDevice() const { return reinterpret_pointer_cast<DXPhysicalDevice>(physicalDevice); }

        auto getDXDevice() const { return reinterpret_pointer_cast<DXDevice>(device); }

        auto getDXGraphicCommandQueue() const { return reinterpret_pointer_cast<DXSubmitQueue>(graphicCommandQueue); }

        auto getDXTransferCommandQueue() const { return reinterpret_pointer_cast<DXSubmitQueue>(transferCommandQueue); }

        auto getDXSwapChain() const { return reinterpret_pointer_cast<DXSwapChain>(swapChain); }

    private:
        HWND hWnd;
        CD3DX12_VIEWPORT viewport;
        CD3DX12_RECT scissorRect;
    };

}