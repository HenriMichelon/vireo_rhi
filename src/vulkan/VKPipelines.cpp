/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/backend/vulkan/Tools.h"
module vireo.vulkan.pipelines;

namespace vireo {

    VKVertexInputLayout::VKVertexInputLayout(size_t size, const vector<AttributeDescription>& attributesDescriptions) {
        vertexBindingDescription.binding = 0;
        vertexBindingDescription.stride = size;
        vertexBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        for (int i = 0; i < attributesDescriptions.size(); i++) {
            vertexAttributeDescriptions.push_back({
                .location = static_cast<uint32_t>(i),
                .binding = 0,
                .format = VKFormat[attributesDescriptions[i].format],
                .offset = attributesDescriptions[i].offset,
            });
        }
    }

    VKShaderModule::VKShaderModule(const VkDevice device, const string& fileName):
        device{device} {
        ifstream file(fileName + ".spv", ios::ate | ios::binary);
        if (!file.is_open()) {
            die("failed to open shader file!");
        }
        const auto fileSize = static_cast<size_t>(file.tellg());
        vector<char> buffer(fileSize);
        file.seekg(0);
        file.read(buffer.data(), fileSize);
        file.close();
        const auto createInfo = VkShaderModuleCreateInfo {
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .codeSize = buffer.size(),
            .pCode = reinterpret_cast<const uint32_t*>(buffer.data())
        };
        DieIfFailed(vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule));
#ifdef _DEBUG
        vkSetObjectName(device, reinterpret_cast<uint64_t>(shaderModule), VK_OBJECT_TYPE_SHADER_MODULE,
            "VKShaderModule : " + fileName);
#endif
    }

    VKShaderModule::~VKShaderModule() {
        vkDestroyShaderModule(device, shaderModule, nullptr);
    }

    VKPipelineResources::VKPipelineResources(
        const VkDevice device,
        const vector<shared_ptr<DescriptorLayout>>& descriptorLayouts,
        const wstring& name):
        device{device} {
        for (const auto& descriptorLayout : descriptorLayouts) {
            const auto layout = static_pointer_cast<VKDescriptorLayout>(descriptorLayout);
            setLayouts.push_back(layout->getSetLayout());
        }
        const auto pipelineLayoutInfo = VkPipelineLayoutCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .setLayoutCount = static_cast<uint32_t>(setLayouts.size()),
            .pSetLayouts = setLayouts.empty() ? nullptr : setLayouts.data(),
            .pushConstantRangeCount = 0,
            .pPushConstantRanges = nullptr,
        };
        DieIfFailed(vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout));
#ifdef _DEBUG
        vkSetObjectName(device, reinterpret_cast<uint64_t>(pipelineLayout), VK_OBJECT_TYPE_PIPELINE_LAYOUT,
            wstring_to_string(L"VKPipelineResources : " + name));
#endif
    }

    VKPipelineResources::~VKPipelineResources() {
        vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
    }

    VKPipeline::VKPipeline(
           VkDevice device,
           VKSwapChain& swapChain,
           PipelineResources& pipelineResources,
           VertexInputLayout& vertexInputLayout,
           ShaderModule& vertexShader,
           ShaderModule& fragmentShader,
           const wstring& name):
        device{device} {
        const auto vertexShaderModule = static_cast<VKShaderModule&>(vertexShader).getShaderModule();
        const auto fragmentShaderModule = static_cast<VKShaderModule&>(fragmentShader).getShaderModule();
        const auto& vkVertexInputLayout = static_cast<VKVertexInputLayout&>(vertexInputLayout);
        const auto& vkPipelineLayout = static_cast<VKPipelineResources&>(pipelineResources);

        const auto shaderStages = vector<VkPipelineShaderStageCreateInfo> {
            {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_VERTEX_BIT,
                .module = vertexShaderModule,
                .pName = "main"
            },
            {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                .module = fragmentShaderModule,
                .pName = "main"
            }
        };
        const auto vertexInputInfo = VkPipelineVertexInputStateCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .vertexBindingDescriptionCount = 1,
            .pVertexBindingDescriptions = &vkVertexInputLayout.getVertexBindingDescription(),
            .vertexAttributeDescriptionCount = static_cast<uint32_t>(vkVertexInputLayout.getVertexAttributeDescription().size()),
            .pVertexAttributeDescriptions = vkVertexInputLayout.getVertexAttributeDescription().data()
        };
        constexpr auto inputAssembly = VkPipelineInputAssemblyStateCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            .primitiveRestartEnable = VK_FALSE,
        };

        const vector dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT_WITH_COUNT,
            VK_DYNAMIC_STATE_SCISSOR_WITH_COUNT,
            VK_DYNAMIC_STATE_CULL_MODE
        };
        const auto dynamicState = VkPipelineDynamicStateCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
            .pDynamicStates = dynamicStates.data()
        };
        constexpr  auto viewportState = VkPipelineViewportStateCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        };
        constexpr auto rasterizer = VkPipelineRasterizationStateCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .depthClampEnable = VK_FALSE,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode = VK_POLYGON_MODE_FILL,
            .frontFace = VK_FRONT_FACE_CLOCKWISE,
            .depthBiasEnable = VK_FALSE,
            .depthBiasConstantFactor = 0.0f,
            .depthBiasClamp = 0.0f,
            .depthBiasSlopeFactor = 0.0f,
            .lineWidth = 1.0f,
        };
        constexpr auto multisampling = VkPipelineMultisampleStateCreateInfo {
            .sType                  = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .rasterizationSamples   = VK_SAMPLE_COUNT_1_BIT,
            .sampleShadingEnable    = VK_FALSE,
            .minSampleShading       = 1.0f,
            .pSampleMask            = nullptr,
            .alphaToCoverageEnable  = VK_FALSE,
            .alphaToOneEnable       = VK_FALSE
        };
        constexpr auto colorBlendAttachment = VkPipelineColorBlendAttachmentState {
            .blendEnable = VK_FALSE,
            // .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
            // .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
            // .colorBlendOp = VK_BLEND_OP_ADD,
            // .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
            // .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
            // .alphaBlendOp = VK_BLEND_OP_ADD,
            .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
        };
        const auto colorBlending = VkPipelineColorBlendStateCreateInfo {
            .sType          = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .logicOpEnable  = VK_FALSE,
            .logicOp        = VK_LOGIC_OP_COPY,
            .attachmentCount= 1,
            .pAttachments   = &colorBlendAttachment,
            .blendConstants = { 0.0f, 0.0f, 0.0f, 0.0f }
        };

        const auto swapChainImageFormat = swapChain.getFormat();
        const auto dynamicRenderingCreateInfo = VkPipelineRenderingCreateInfoKHR{
            .sType                   = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR,
            .pNext                   = VK_NULL_HANDLE,
            .colorAttachmentCount    = 1,
            .pColorAttachmentFormats = &swapChainImageFormat,
        };

        const auto pipelineInfo = VkGraphicsPipelineCreateInfo {
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .pNext = &dynamicRenderingCreateInfo,
            .flags = 0,
            .stageCount = static_cast<uint32_t>(shaderStages.size()),
            .pStages = shaderStages.data(),
            .pVertexInputState = &vertexInputInfo,
            .pInputAssemblyState = &inputAssembly,
            .pViewportState = &viewportState,
            .pRasterizationState = &rasterizer,
            .pMultisampleState = &multisampling,
            .pDepthStencilState = nullptr,
            .pColorBlendState = &colorBlending,
            .pDynamicState = &dynamicState,
            .layout = vkPipelineLayout.getPipelineLayout(),
            .renderPass = VK_NULL_HANDLE,
            .subpass = 0,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = -1,
        };
        DieIfFailed(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline));
#ifdef _DEBUG
        vkSetObjectName(device, reinterpret_cast<uint64_t>(pipeline), VK_OBJECT_TYPE_PIPELINE,
            wstring_to_string(L"VKPipeline : " + name));
#endif
    }

    VKPipeline::~VKPipeline() {
        vkDestroyPipeline(device, pipeline, nullptr);
    }

}