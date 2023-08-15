#include "VulkanGraphicPipelineEngine.h"
#include "Core.h"

#pragma region Public

VulkanGraphicPipelineEngine::VulkanGraphicPipelineEngine(Core* core)
{
	pCore = core;
}

VulkanGraphicPipelineEngine::~VulkanGraphicPipelineEngine()
{
}

void VulkanGraphicPipelineEngine::createGraphicsPipeline()
{    
    auto vertShaderCode = pCore->pFileStreamEngine->readFile("shaders/vert.spv");
    auto fragShaderCode = pCore->pFileStreamEngine->readFile("shaders/frag.spv");

    VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
    VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

    //.....
    
    vkDestroyShaderModule(*(pCore->pVulkanDeviceEngine->pDevice), fragShaderModule, nullptr);
    vkDestroyShaderModule(*(pCore->pVulkanDeviceEngine->pDevice), vertShaderModule, nullptr);
}

#pragma endregion

#pragma region Private

VkShaderModule VulkanGraphicPipelineEngine::createShaderModule(const std::vector<char>& code) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(*(pCore->pVulkanDeviceEngine->pDevice), &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module!");
    }

    return shaderModule;
}
#pragma endregion