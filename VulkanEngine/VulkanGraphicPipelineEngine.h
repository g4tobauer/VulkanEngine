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
private:
	VkPipelineLayout pipelineLayout;
	VkShaderModule createShaderModule(const std::vector<char>& code);
};
#endif // !ENGINE_VULKAN_GRAPHIC_PIPELINE