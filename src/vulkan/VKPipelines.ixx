/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/backend/vulkan/Tools.h"
export module vireo.vulkan.pipelines;

import vireo;
import vireo.vulkan.descriptors;
import vireo.vulkan.swapchains;

export namespace vireo {

    class VKVertexInputLayout : public VertexInputLayout {
    public:
        static constexpr VkFormat VKFormat[] {
            VK_FORMAT_R32G32_SFLOAT,
            VK_FORMAT_R32G32B32_SFLOAT,
            VK_FORMAT_R32G32B32A32_SFLOAT
        };

        VKVertexInputLayout(size_t size, const vector<AttributeDescription>& attributesDescriptions);

        const auto& getVertexBindingDescription() const { return vertexBindingDescription; }

        const auto& getVertexAttributeDescription() const { return vertexAttributeDescriptions; }

    private:
        VkVertexInputBindingDescription                vertexBindingDescription;
        vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions;
    };

    class VKShaderModule: public ShaderModule {
    public:
        VKShaderModule(VkDevice device, const string& fileName);

        ~VKShaderModule() override;

        auto getShaderModule() const { return shaderModule; }

    private:
        VkDevice       device;
        VkShaderModule shaderModule;
    };

    class VKPipelineResources : public PipelineResources {
    public:
        VKPipelineResources(
            VkDevice device,
            const vector<shared_ptr<DescriptorLayout>>& descriptorLayouts,
            const wstring& name);

        ~VKPipelineResources() override;

        auto getPipelineLayout() const { return pipelineLayout; }

        const auto& getSetLayouts() const { return setLayouts; }

    private:
        VkDevice device;
        VkPipelineLayout pipelineLayout;
        vector<VkDescriptorSetLayout> setLayouts;
    };

    class VKPipeline : public Pipeline {
    public:
        static constexpr VkCullModeFlagBits vkCullMode[] {
            VK_CULL_MODE_NONE,
            VK_CULL_MODE_FRONT_BIT,
            VK_CULL_MODE_BACK_BIT
        };

        VKPipeline(
           VkDevice device,
           VKSwapChain& swapChain,
           const shared_ptr<PipelineResources>& pipelineResources,
           const shared_ptr<const VertexInputLayout>& vertexInputLayout,
           const shared_ptr<const ShaderModule>& vertexShader,
           const shared_ptr<const ShaderModule>& fragmentShader,
           const Pipeline::Configuration& configuration,
           const wstring& name);

        auto getPipeline() const { return pipeline; }

        ~VKPipeline() override;

    private:
        VkDevice     device;
        VkPipeline   pipeline;

        static constexpr auto colorBlendAttachmentEnable = VkPipelineColorBlendAttachmentState {
            .blendEnable = VK_TRUE,
            .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
            .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
            .colorBlendOp = VK_BLEND_OP_ADD,
            .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
            .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
            .alphaBlendOp = VK_BLEND_OP_ADD,
            .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
        };
        static constexpr auto colorBlendAttachmentDisable = VkPipelineColorBlendAttachmentState {
            .blendEnable = VK_FALSE,
            .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
        };
        static constexpr auto inputAssembly = VkPipelineInputAssemblyStateCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            .primitiveRestartEnable = VK_FALSE,
        };
        static constexpr  auto viewportState = VkPipelineViewportStateCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        };

    };

}