/*
* Copyright (c) 2025-present Henri Michelon
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
module;
#include "vireo/backend/vulkan/Libraries.h"
#include <cassert>
module vireo.vulkan.pipelines;

import std;
import vireo.tools;
import vireo.vulkan.devices;
import vireo.vulkan.resources;
import vireo.vulkan.tools;

namespace vireo {

    VKVertexInputLayout::VKVertexInputLayout(const size_t size, const std::vector<VertexAttributeDesc>& attributesDescriptions) {
        vertexBindingDescription.binding = 0;
        vertexBindingDescription.stride = size;
        vertexBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        for (int i = 0; i < attributesDescriptions.size(); i++) {
            vertexAttributeDescriptions.push_back({
                .location = static_cast<uint32_t>(i),
                .binding = 0,
                .format = VKFormat[static_cast<int>(attributesDescriptions[i].format)],
                .offset = attributesDescriptions[i].offset,
            });
        }
    }

    VKShaderModule::VKShaderModule(const VkDevice device, const std::string& fileName):
        device{device} {
        assert(device != VK_NULL_HANDLE);
        assert(!fileName.empty());
        std::ifstream file(fileName + ".spv", std::ios::ate | std::ios::binary);
        if (!file.is_open()) {
            throw Exception("failed to open shader file ", fileName);
        }
        const auto fileSize = static_cast<size_t>(file.tellg());
        file.seekg(0);
        load(file, fileSize, fileName);
        file.close();
    }

    VKShaderModule::VKShaderModule(const VkDevice device, std::ifstream inputStream, const size_t size) :
        device{device} {
        assert(device != VK_NULL_HANDLE);
        load(inputStream, size, "");
    }

    void VKShaderModule::load(std::ifstream& inputStream, const size_t size, const std::string& fileName) {
        std::vector<char> buffer(size);
        inputStream.read(buffer.data(), size);
        const auto createInfo = VkShaderModuleCreateInfo {
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .codeSize = buffer.size(),
            .pCode = reinterpret_cast<const uint32_t*>(buffer.data()),
        };
        vkCheck(vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule));
#ifdef _DEBUG
        vkSetObjectName(device, reinterpret_cast<uint64_t>(shaderModule), VK_OBJECT_TYPE_SHADER_MODULE,
            "VKShaderModule : " + fileName);
#endif
    }

    VKShaderModule::VKShaderModule(const VkDevice device, const std::vector<char>& data) :
        device{device} {
        const auto createInfo = VkShaderModuleCreateInfo {
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .codeSize = data.size(),
            .pCode = reinterpret_cast<const uint32_t*>(data.data()),
        };
        vkCheck(vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule));
#ifdef _DEBUG
        vkSetObjectName(device, reinterpret_cast<uint64_t>(shaderModule), VK_OBJECT_TYPE_SHADER_MODULE,
            "VKShaderModule");
#endif
    }


    VKShaderModule::~VKShaderModule() {
        vkDestroyShaderModule(device, shaderModule, nullptr);
    }

    VKPipelineResources::VKPipelineResources(
        const VkDevice device,
        const std::vector<std::shared_ptr<DescriptorLayout>>& descriptorLayouts,
        const PushConstantsDesc& pushConstant,
        const std::string& name):
        device{device} {
        assert(device != VK_NULL_HANDLE);
        for (const auto& descriptorLayout : descriptorLayouts) {
            const auto layout = static_pointer_cast<const VKDescriptorLayout>(descriptorLayout);
            setLayouts.push_back(layout->getSetLayout());
        }
        auto pipelineLayoutInfo = VkPipelineLayoutCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .setLayoutCount = static_cast<uint32_t>(setLayouts.size()),
            .pSetLayouts = setLayouts.empty() ? nullptr : setLayouts.data(),
        };
        auto pushConstantRange = VkPushConstantRange {};
        if (pushConstant.size == 0) {
            pipelineLayoutInfo.pushConstantRangeCount = 0;
            pipelineLayoutInfo.pPushConstantRanges = nullptr;
        } else {
            if (pushConstant.stage == ShaderStage::VERTEX) {
                pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
            } else if (pushConstant.stage == ShaderStage::FRAGMENT) {
                pushConstantRange.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
            }  else {
                pushConstantRange.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
            }
            pushConstantRange.offset = pushConstant.offset;
            pushConstantRange.size = pushConstant.size;
            pipelineLayoutInfo.pushConstantRangeCount = 1;
            pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
        }
        vkCheck(vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout));
#ifdef _DEBUG
        vkSetObjectName(device, reinterpret_cast<uint64_t>(pipelineLayout), VK_OBJECT_TYPE_PIPELINE_LAYOUT,
            "VKPipelineResources : " + name);
#endif
    }

    VKPipelineResources::~VKPipelineResources() {
        vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
    }

    VKComputePipeline::VKComputePipeline(
          const VkDevice device,
          const std::shared_ptr<PipelineResources>& pipelineResources,
          const std::shared_ptr<const ShaderModule>& shader,
          const std::string& name) :
        ComputePipeline{pipelineResources},
        device{device} {
        assert(device != VK_NULL_HANDLE);
        assert(shader != nullptr);
        const auto shaderModule = static_pointer_cast<const VKShaderModule>(shader)->getShaderModule();
        const auto& pipelineLayout = static_pointer_cast<const VKPipelineResources>(pipelineResources)->getPipelineLayout();

        const auto shaderStage = VkPipelineShaderStageCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .stage = VK_SHADER_STAGE_COMPUTE_BIT,
            .module = shaderModule,
            .pName = "main",
            .pSpecializationInfo = nullptr,
        };
        const auto createInfo = VkComputePipelineCreateInfo {
            .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
            .stage = shaderStage,
            .layout = pipelineLayout,
        };
        vkCheck(vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, &createInfo, nullptr, &pipeline));
#ifdef _DEBUG
        vkSetObjectName(device, reinterpret_cast<uint64_t>(pipeline), VK_OBJECT_TYPE_PIPELINE,
            "VKComputePipeline : " + name);
#endif
    }

    VKComputePipeline::~VKComputePipeline() {
        vkDestroyPipeline(device, pipeline, nullptr);
    }

    VKGraphicPipeline::VKGraphicPipeline(
           const std::shared_ptr<VKDevice>& device,
           const GraphicPipelineConfiguration& configuration,
           const std::string& name):
        GraphicPipeline{configuration.resources},
        device{device} {
        assert(configuration.resources != nullptr);
        assert(configuration.vertexShader != nullptr);
        assert(configuration.colorRenderFormats.size() == configuration.colorBlendDesc.size());
        const auto& vkPipelineLayout = static_pointer_cast<const VKPipelineResources>(configuration.resources);

        auto shaderStages = std::vector<VkPipelineShaderStageCreateInfo>{};
        shaderStages.push_back({
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_VERTEX_BIT,
            .module = static_pointer_cast<const VKShaderModule>(configuration.vertexShader)->getShaderModule(),
            .pName = "main",
        });
        if (configuration.fragmentShader) {
            shaderStages.push_back({
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                .module = static_pointer_cast<const VKShaderModule>(configuration.fragmentShader)->getShaderModule(),
                .pName = "main"
            });
        }
        if (configuration.hullShader) {
            shaderStages.push_back({
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT,
                .module = static_pointer_cast<const VKShaderModule>(configuration.fragmentShader)->getShaderModule(),
                .pName = "main"
            });
        }
        if (configuration.domainShader) {
            shaderStages.push_back({
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,
                .module = static_pointer_cast<const VKShaderModule>(configuration.fragmentShader)->getShaderModule(),
                .pName = "main"
            });
        }
        if (configuration.geometryShader) {
            shaderStages.push_back({
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_GEOMETRY_BIT,
                .module = static_pointer_cast<const VKShaderModule>(configuration.fragmentShader)->getShaderModule(),
                .pName = "main"
            });
        }
        auto vertexInputInfo = VkPipelineVertexInputStateCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        };
        if (configuration.vertexInputLayout) {
            const auto& vkVertexInputLayout = static_pointer_cast<const VKVertexInputLayout>(configuration.vertexInputLayout);
            vertexInputInfo.vertexBindingDescriptionCount = 1;
            vertexInputInfo.pVertexBindingDescriptions = &vkVertexInputLayout->getVertexBindingDescription();
            vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vkVertexInputLayout->getVertexAttributeDescription().size());
            vertexInputInfo.pVertexAttributeDescriptions = vkVertexInputLayout->getVertexAttributeDescription().data();
        }

        const std::vector dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT_WITH_COUNT,
            VK_DYNAMIC_STATE_SCISSOR_WITH_COUNT,
            VK_DYNAMIC_STATE_STENCIL_REFERENCE,
        };
        const auto dynamicState = VkPipelineDynamicStateCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
            .pDynamicStates = dynamicStates.data()
        };
        const auto rasterizer = VkPipelineRasterizationStateCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .depthClampEnable = VK_FALSE,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode = configuration.polygonMode == PolygonMode::FILL ? VK_POLYGON_MODE_FILL : VK_POLYGON_MODE_LINE,
            .cullMode = static_cast<VkCullModeFlags>(vkCullMode[static_cast<int>(configuration.cullMode)]),
            .frontFace = configuration.frontFaceCounterClockwise ?  VK_FRONT_FACE_COUNTER_CLOCKWISE : VK_FRONT_FACE_CLOCKWISE,
            .depthBiasEnable = configuration.depthBiasEnable,
            .depthBiasConstantFactor = configuration.depthBiasConstantFactor,
            .depthBiasClamp = configuration.depthBiasClamp,
            .depthBiasSlopeFactor = configuration.depthBiasSlopeFactor ,
            .lineWidth = 1.0f,
        };
        const auto depthStencil = VkPipelineDepthStencilStateCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
            .depthTestEnable = configuration.depthTestEnable,
            .depthWriteEnable = configuration.depthWriteEnable,
            .depthCompareOp = vkCompareOp[static_cast<int>(configuration.depthCompareOp)],
            .stencilTestEnable = configuration.stencilTestEnable,
            .front = {
                .failOp = vkStencilOp[static_cast<int>(configuration.frontStencilOpState.failOp)],
                .passOp = vkStencilOp[static_cast<int>(configuration.frontStencilOpState.passOp)],
                .depthFailOp = vkStencilOp[static_cast<int>(configuration.frontStencilOpState.failOp)],
                .compareOp = vkCompareOp[static_cast<int>(configuration.frontStencilOpState.compareOp)],
                .compareMask = configuration.frontStencilOpState.compareMask,
                .writeMask = configuration.frontStencilOpState.writeMask,
            },
            .back = {
                .failOp = vkStencilOp[static_cast<int>(configuration.backStencilOpState.failOp)],
                .passOp = vkStencilOp[static_cast<int>(configuration.backStencilOpState.passOp)],
                .depthFailOp = vkStencilOp[static_cast<int>(configuration.backStencilOpState.failOp)],
                .compareOp = vkCompareOp[static_cast<int>(configuration.backStencilOpState.compareOp)],
                .compareMask = configuration.backStencilOpState.compareMask,
                .writeMask = configuration.backStencilOpState.writeMask,
            },
        };
        const auto multisampling = VkPipelineMultisampleStateCreateInfo {
            .sType                  = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .rasterizationSamples   = VKPhysicalDevice::vkSampleCountFlag[static_cast<int>(configuration.msaa)],
            .sampleShadingEnable    = VK_FALSE,
            .minSampleShading       = 1.0f,
            .pSampleMask            = nullptr,
            .alphaToCoverageEnable  = configuration.alphaToCoverageEnable,
            .alphaToOneEnable       = VK_FALSE
        };
        const auto blendStateViews = configuration.colorBlendDesc | std::views::transform([](const ColorBlendDesc& desc) {
            return VkPipelineColorBlendAttachmentState {
            .blendEnable         = desc.blendEnable ? VK_TRUE : VK_FALSE,
            .srcColorBlendFactor = vkBlendFactor[static_cast<size_t>(desc.srcColorBlendFactor)],
            .dstColorBlendFactor = vkBlendFactor[static_cast<size_t>(desc.dstColorBlendFactor)],
            .colorBlendOp        = vkBlendOp[static_cast<size_t>(desc.colorBlendOp)],
            .srcAlphaBlendFactor = vkBlendFactor[static_cast<size_t>(desc.srcAlphaBlendFactor)],
            .dstAlphaBlendFactor = vkBlendFactor[static_cast<size_t>(desc.dstAlphaBlendFactor)],
            .alphaBlendOp        = vkBlendOp[static_cast<size_t>(desc.alphaBlendOp)],
            .colorWriteMask      = static_cast<VkColorComponentFlags>(desc.colorWriteMask)};
        });
        const auto colorBlendStates = std::vector<VkPipelineColorBlendAttachmentState> {blendStateViews.begin(), blendStateViews.end()};
        const auto colorBlending = VkPipelineColorBlendStateCreateInfo {
            .sType          = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .logicOpEnable  = configuration.logicOpEnable ? VK_TRUE : VK_FALSE,
            .logicOp        = vkLogicOp[static_cast<size_t>(configuration.logicOp)],
            .attachmentCount= static_cast<uint32_t>(colorBlendStates.size()),
            .pAttachments   = colorBlendStates.data(),

        };
        const auto formatsView = configuration.colorRenderFormats | std::views::transform([](const ImageFormat& format) {
            return VKImage::vkFormats[static_cast<int>(format)];
        });
        const auto formats = std::vector<VkFormat> {formatsView.begin(), formatsView.end()};
        const auto dynamicRenderingCreateInfo = VkPipelineRenderingCreateInfo{
            .sType                   = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR,
            .pNext                   = VK_NULL_HANDLE,
            .colorAttachmentCount    = static_cast<uint32_t>(formats.size()),
            .pColorAttachmentFormats = formats.data(),
            .depthAttachmentFormat   = configuration.depthTestEnable ?
                VKImage::vkFormats[static_cast<int>(configuration.depthStencilImageFormat)]:
                VK_FORMAT_UNDEFINED,
            .stencilAttachmentFormat = configuration.stencilTestEnable ?
                VKImage::vkFormats[static_cast<int>(configuration.depthStencilImageFormat)] :
                VK_FORMAT_UNDEFINED,
        };
        const auto IAInfo = VkPipelineInputAssemblyStateCreateInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .topology = vkPrimitives[static_cast<int>(configuration.primitiveTopology)],
        };
        const auto pipelineInfo = VkGraphicsPipelineCreateInfo {
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .pNext = &dynamicRenderingCreateInfo,
            .flags = 0,
            .stageCount = static_cast<uint32_t>(shaderStages.size()),
            .pStages = shaderStages.data(),
            .pVertexInputState = &vertexInputInfo,
            .pInputAssemblyState = &IAInfo,
            .pViewportState = nullptr,
            .pRasterizationState = &rasterizer,
            .pMultisampleState = &multisampling,
            .pDepthStencilState = &depthStencil,
            .pColorBlendState = &colorBlending,
            .pDynamicState = &dynamicState,
            .layout = vkPipelineLayout->getPipelineLayout(),
            .renderPass = VK_NULL_HANDLE,
            .subpass = 0,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = -1,
        };
        vkCheck(vkCreateGraphicsPipelines(device->getDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline));
#ifdef _DEBUG
        vkSetObjectName(device->getDevice(), reinterpret_cast<uint64_t>(pipeline), VK_OBJECT_TYPE_PIPELINE,
            "VKGraphicPipeline : " + name);
#endif
    }

    VKGraphicPipeline::~VKGraphicPipeline() {
        vkDestroyPipeline(device->getDevice(), pipeline, nullptr);
    }

}