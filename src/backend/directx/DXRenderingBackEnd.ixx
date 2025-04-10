/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "backend/directx/Libraries.h"

export module vireo.backend.directx;

import vireo.backend;
import vireo.backend.buffer;
import vireo.backend.descriptors;
import vireo.backend.framedata;
import vireo.backend.samplers;

import vireo.backend.directx.device;

export namespace vireo::backend {

    class DXSubmitQueue : public SubmitQueue{
    public:
        DXSubmitQueue(CommandList::Type type, const ComPtr<ID3D12Device>& device);

        auto getCommandQueue() { return commandQueue; }

        void submit(const FrameData& frameData, std::vector<std::shared_ptr<CommandList>> commandLists) override;

        void submit(std::vector<std::shared_ptr<CommandList>> commandLists) override;

        void waitIdle() override;

    private:
        ComPtr<ID3D12Device>       device;
        ComPtr<ID3D12CommandQueue> commandQueue;
    };

    class DXCommandAllocator : public CommandAllocator {
    public:
        DXCommandAllocator(CommandList::Type type, const ComPtr<ID3D12Device>& device);

        std::shared_ptr<CommandList> createCommandList(Pipeline& pipeline) const override;

        std::shared_ptr<CommandList> createCommandList() const override;

    private:
        ComPtr<ID3D12Device>           device;
        ComPtr<ID3D12CommandAllocator> commandAllocator;
    };

    class DXCommandList : public CommandList {
    public:
        static constexpr D3D12_COMMAND_LIST_TYPE ListType[] {
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            D3D12_COMMAND_LIST_TYPE_DIRECT, //D3D12_COMMAND_LIST_TYPE_COPY,
            D3D12_COMMAND_LIST_TYPE_COMPUTE,
        };

        DXCommandList(
            Type type,
            const ComPtr<ID3D12Device>& device,
            const ComPtr<ID3D12CommandAllocator>& commandAllocator,
            const ComPtr<ID3D12PipelineState>& pipelineState = nullptr);

        void reset() override;

        void begin(Pipeline& pipeline) override;

        void begin() override;

        void end() override;

        void bindVertexBuffer(Buffer& buffer) override;

        void drawInstanced(uint32_t vertexCountPerInstance, uint32_t instanceCount = 1) override;

        void upload(Buffer& destination, const void* source) override;

        void cleanup() override;

        auto getCommandList() { return commandList; }

        ~DXCommandList() override = default;

    private:
        ComPtr<ID3D12Device>                device;
        ComPtr<ID3D12GraphicsCommandList>   commandList;
        ComPtr<ID3D12CommandAllocator>      commandAllocator;
        std::vector<ComPtr<ID3D12Resource>> stagingBuffers{};
    };

    class DXSwapChain : public SwapChain {
    public:
        static constexpr auto RENDER_FORMAT{DXGI_FORMAT_R8G8B8A8_UNORM_SRGB};

        DXSwapChain(
            const ComPtr<IDXGIFactory4>& factory,
            DXDevice& device,
            const ComPtr<ID3D12CommandQueue>& commandQueue,
            uint32_t width, uint32_t height);

        auto getSwapChain() { return swapChain; }

        auto getRenderTargets() { return renderTargets; }

        auto getDescriptorSize() const {  return rtvDescriptorSize; }

        auto getHeap() { return rtvHeap; }

        void nextSwapChain() override;

        bool acquire(FrameData& frameData) override;

        void present(FrameData& frameData) override;

    private:
        DXDevice&                    device;
        ComPtr<IDXGISwapChain3>      swapChain;
        ComPtr<ID3D12CommandQueue>   presentCommandQueue;
        ComPtr<ID3D12Resource>       renderTargets[FRAMES_IN_FLIGHT];
        ComPtr<ID3D12DescriptorHeap> rtvHeap;
        UINT                         rtvDescriptorSize{0};
    };

    class DXVertexInputLayout : public VertexInputLayout {
    public:
        static constexpr DXGI_FORMAT DXFormat[] {
            DXGI_FORMAT_R32G32_FLOAT,
            DXGI_FORMAT_R32G32B32_FLOAT,
            DXGI_FORMAT_R32G32B32A32_FLOAT
        };

        DXVertexInputLayout(const std::vector<AttributeDescription>& attributesDescriptions);

        const auto& getInputElementDescs() const { return inputElementDescs; }

    private:
        std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDescs;
    };

    class DXShaderModule: public ShaderModule {
    public:
        DXShaderModule(const std::string& fileName);

        auto getShader() const { return shader; }

    private:
        ComPtr<ID3DBlob> shader;
    };

    class DXPipelineResources : public PipelineResources {
    public:
        DXPipelineResources(const ComPtr<ID3D12Device>& device, const std::vector<std::shared_ptr<Sampler>>& staticSamplers, const std::wstring& name);

        auto getRootSignature() const { return rootSignature; }

    private:
        ComPtr<ID3D12RootSignature> rootSignature;
    };

    class DXPipeline : public Pipeline {
    public:
        DXPipeline(
            const ComPtr<ID3D12Device>& device,
            PipelineResources& pipelineResources,
            VertexInputLayout& vertexInputLayout,
            ShaderModule& vertexShader,
            ShaderModule& fragmentShader,
            const std::wstring& name);

        auto getPipelineState() { return pipelineState; }

    private:
        ComPtr<ID3D12PipelineState> pipelineState;
    };

    class DXRenderingBackEnd : public RenderingBackEnd {
    public:
        DXRenderingBackEnd();

        void waitIdle() override;

        std::shared_ptr<CommandAllocator> createCommandAllocator(CommandList::Type type) const override;

        std::shared_ptr<FrameData> createFrameData(uint32_t frameIndex) override;

        std::shared_ptr<VertexInputLayout> createVertexLayout(
            size_t size,
            const std::vector<VertexInputLayout::AttributeDescription>& attributesDescriptions) const override;

        std::shared_ptr<ShaderModule> createShaderModule(const std::string& fileName) const override;

        std::shared_ptr<PipelineResources> createPipelineResources(
            const std::vector<std::shared_ptr<Sampler>>& staticSamplers,
            const std::wstring& name = L"PipelineResource") const override;

        std::shared_ptr<Pipeline> createPipeline(
            PipelineResources& pipelineResources,
            VertexInputLayout& vertexInputLayout,
            ShaderModule& vertexShader,
            ShaderModule& fragmentShader,
            const std::wstring& name = L"Pipeline") const override;

        std::shared_ptr<Buffer> createBuffer(Buffer::Type type, size_t size, size_t count = 1, size_t alignment = 1, const std::wstring& name = L"Buffer") const override;

        std::shared_ptr<DescriptorSet> createDescriptorSet(DescriptorType type, uint32_t capacity) override;

        std::shared_ptr<Sampler> createSampler(
               Filter minFilter,
               Filter magFilter,
               AddressMode addressModeU,
               AddressMode addressModeV,
               AddressMode addressModeW,
               float minLod = 0.0f,
               float maxLod = 1.0f,
               bool anisotropyEnable = true,
               MipMapMode mipMapMode = MipMapMode::LINEAR) const override;

        void beginRendering(FrameData& frameData, PipelineResources& pipelineResources, Pipeline& pipeline, CommandList& commandList) override;

        void endRendering(CommandList& commandList) override;

        auto getDXInstance() const { return std::reinterpret_pointer_cast<DXInstance>(instance); }

        auto getDXPhysicalDevice() const { return std::reinterpret_pointer_cast<DXPhysicalDevice>(physicalDevice); }

        auto getDXDevice() const { return std::reinterpret_pointer_cast<DXDevice>(device); }

        auto getDXGraphicCommandQueue() const { return std::reinterpret_pointer_cast<DXSubmitQueue>(graphicCommandQueue); }

        auto getDXTransferCommandQueue() const { return std::reinterpret_pointer_cast<DXSubmitQueue>(transferCommandQueue); }

        auto getDXSwapChain() const { return std::reinterpret_pointer_cast<DXSwapChain>(swapChain); }

    private:
        CD3DX12_VIEWPORT viewport;
        CD3DX12_RECT scissorRect;
    };

}