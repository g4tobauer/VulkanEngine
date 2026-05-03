#pragma once
#ifndef ENGINE_VULKAN_GRAPHIC_PIPELINE
#define ENGINE_VULKAN_GRAPHIC_PIPELINE

#include "BaseEngine.h"

class VulkanGraphicPipelineEngine : BaseEngine
{
public:
	VulkanGraphicPipelineEngine(Core* core);
	~VulkanGraphicPipelineEngine();

	void createGraphicsPipeline();
	void destroyGraphicsPipeline();
	VkPipeline graphicsPipelineHandle() const;
private:
	VkPipeline *pGraphicsPipeline;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;
	
	VkShaderModule createShaderModule(const std::vector<char>& code);
};
#endif // !ENGINE_VULKAN_GRAPHIC_PIPELINE
