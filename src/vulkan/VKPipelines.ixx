/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/backend/vulkan/Libraries.h"
export module vireo.vulkan.pipelines;

import vireo;
import vireo.vulkan.descriptors;
import vireo.vulkan.devices;
import vireo.vulkan.swapchains;

export namespace vireo {

    class VKVertexInputLayout : public VertexInputLayout {
    public:
        static constexpr VkFormat VKFormat[] {
            VK_FORMAT_R32_SFLOAT,
            VK_FORMAT_R32G32_SFLOAT,
            VK_FORMAT_R32G32B32_SFLOAT,
            VK_FORMAT_R32G32B32A32_SFLOAT,
            VK_FORMAT_R32_SINT,
            VK_FORMAT_R32G32_SINT,
            VK_FORMAT_R32G32B32_SINT,
            VK_FORMAT_R32G32B32A32_SINT,
            VK_FORMAT_R32_UINT,
            VK_FORMAT_R32G32_UINT,
            VK_FORMAT_R32G32B32_UINT,
            VK_FORMAT_R32G32B32A32_UINT,
        };

        VKVertexInputLayout(size_t size, const std::vector<VertexAttributeDesc>& attributesDescriptions);

        const auto& getVertexBindingDescription() const { return vertexBindingDescription; }

        const auto& getVertexAttributeDescription() const { return vertexAttributeDescriptions; }

    private:
        VkVertexInputBindingDescription                vertexBindingDescription;
        std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions;
    };

    class VKShaderModule: public ShaderModule {
    public:
        VKShaderModule(VkDevice device, const std::string& fileName);

        VKShaderModule(VkDevice device, std::ifstream inputStream, size_t size);

        VKShaderModule(const VkDevice device, const std::vector<char>& data);

        ~VKShaderModule() override;

        auto getShaderModule() const { return shaderModule; }

    private:
        VkDevice       device;
        VkShaderModule shaderModule;

        void load(std::ifstream& inputStream, size_t size, const std::string& fileName);
    };

    class VKPipelineResources : public PipelineResources {
    public:
        VKPipelineResources(
            VkDevice device,
            const std::vector<std::shared_ptr<DescriptorLayout>>& descriptorLayouts,
            const PushConstantsDesc& pushConstant,
            const std::wstring& name);

        ~VKPipelineResources() override;

        auto getPipelineLayout() const { return pipelineLayout; }

        const auto& getSetLayouts() const { return setLayouts; }

    private:
        VkDevice device;
        VkPipelineLayout pipelineLayout;
        std::vector<VkDescriptorSetLayout> setLayouts;
    };

    class VKComputePipeline : public ComputePipeline {
    public:
        VKComputePipeline(
           VkDevice device,
           const std::shared_ptr<PipelineResources>& pipelineResources,
           const std::shared_ptr<const ShaderModule>& shader,
           const std::wstring& name);

        auto getPipeline() const { return pipeline; }

        ~VKComputePipeline() override;

    private:
        VkDevice     device;
        VkPipeline   pipeline;
    };

    class VKGraphicPipeline : public GraphicPipeline {
    public:
        static constexpr VkCullModeFlagBits vkCullMode[] {
            VK_CULL_MODE_NONE,
            VK_CULL_MODE_FRONT_BIT,
            VK_CULL_MODE_BACK_BIT
        };
        static constexpr VkCompareOp vkCompareOp[] {
            VK_COMPARE_OP_NEVER,
            VK_COMPARE_OP_LESS,
            VK_COMPARE_OP_EQUAL,
            VK_COMPARE_OP_LESS_OR_EQUAL,
            VK_COMPARE_OP_GREATER,
            VK_COMPARE_OP_NOT_EQUAL,
            VK_COMPARE_OP_GREATER_OR_EQUAL,
            VK_COMPARE_OP_ALWAYS,
        };
        static constexpr VkPrimitiveTopology vkPrimitives[]{
            VK_PRIMITIVE_TOPOLOGY_POINT_LIST,
            VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
            VK_PRIMITIVE_TOPOLOGY_LINE_STRIP,
            VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP
        };
        static constexpr VkBlendFactor vkBlendFactor[] = {
            VK_BLEND_FACTOR_ZERO,                   // ZERO
            VK_BLEND_FACTOR_ONE,                    // ONE
            VK_BLEND_FACTOR_SRC_COLOR,              // SRC_COLOR
            VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR,    // ONE_MINUS_SRC_COLOR
            VK_BLEND_FACTOR_DST_COLOR,              // DST_COLOR
            VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR,    // ONE_MINUS_DST_COLOR
            VK_BLEND_FACTOR_SRC_ALPHA,              // SRC_ALPHA
            VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,    // ONE_MINUS_SRC_ALPHA
            VK_BLEND_FACTOR_DST_ALPHA,              // DST_ALPHA
            VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA,    // ONE_MINUS_DST_ALPHA
            VK_BLEND_FACTOR_CONSTANT_COLOR,         // CONSTANT_COLOR
            VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR, // ONE_MINUS_CONSTANT_COLOR
            VK_BLEND_FACTOR_CONSTANT_ALPHA,         // CONSTANT_ALPHA
            VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA, // ONE_MINUS_CONSTANT_ALPHA
            VK_BLEND_FACTOR_SRC_ALPHA_SATURATE,     // SRC_ALPHA_SATURATE
            VK_BLEND_FACTOR_SRC1_COLOR,             // SRC1_COLOR
            VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR,   // ONE_MINUS_SRC1_COLOR
            VK_BLEND_FACTOR_SRC1_ALPHA,             // SRC1_ALPHA
            VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA    // ONE_MINUS_SRC1_ALPHA
        };
        static constexpr VkBlendOp vkBlendOp[] = {
            VK_BLEND_OP_ADD,                 // ADD
            VK_BLEND_OP_SUBTRACT,            // SUBTRACT
            VK_BLEND_OP_REVERSE_SUBTRACT,    // REVERSE_SUBTRACT
            VK_BLEND_OP_MIN,                 // MIN
            VK_BLEND_OP_MAX                  // MAX
        };
        static constexpr VkLogicOp vkLogicOp[] = {
            VK_LOGIC_OP_CLEAR,          // CLEAR
            VK_LOGIC_OP_SET,            // SET
            VK_LOGIC_OP_COPY,           // COPY
            VK_LOGIC_OP_COPY_INVERTED,  // COPY_INVERTED
            VK_LOGIC_OP_NO_OP,          // NOOP
            VK_LOGIC_OP_INVERT,         // INVERT
            VK_LOGIC_OP_AND,            // AND
            VK_LOGIC_OP_NAND,           // NAND
            VK_LOGIC_OP_OR,             // OR
            VK_LOGIC_OP_NOR,            // NOR
            VK_LOGIC_OP_XOR,            // XOR
            VK_LOGIC_OP_EQUIVALENT,     // EQUIV
            VK_LOGIC_OP_AND_REVERSE,    // AND_REVERSE
            VK_LOGIC_OP_AND_INVERTED,   // AND_INVERTED
            VK_LOGIC_OP_OR_REVERSE,     // OR_REVERSE
            VK_LOGIC_OP_OR_INVERTED     // OR_INVERTED
        };
        static constexpr VkStencilOp vkStencilOp[] = {
            VK_STENCIL_OP_KEEP,
            VK_STENCIL_OP_ZERO,
            VK_STENCIL_OP_REPLACE,
            VK_STENCIL_OP_INCREMENT_AND_CLAMP,
            VK_STENCIL_OP_DECREMENT_AND_CLAMP,
            VK_STENCIL_OP_INVERT,
            VK_STENCIL_OP_INCREMENT_AND_WRAP,
            VK_STENCIL_OP_DECREMENT_AND_WRAP,
        };

        VKGraphicPipeline(
           const std::shared_ptr<VKDevice>& device,
           const GraphicPipelineConfiguration& configuration,
           const std::wstring& name);

        auto getPipeline() const { return pipeline; }

        ~VKGraphicPipeline() override;

    private:
        const std::shared_ptr<VKDevice> device;
        VkPipeline pipeline;
    };

}