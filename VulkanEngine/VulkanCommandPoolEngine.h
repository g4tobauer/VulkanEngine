#pragma once
#ifndef ENGINE_VULKAN_COMMAND_POOL
#define ENGINE_VULKAN_COMMAND_POOL

#include "BaseEngine.h"

class VulkanCommandPoolEngine : BaseEngine
{
public:
	VulkanCommandPoolEngine(Core* core);
	~VulkanCommandPoolEngine();
	
	void createCommandPool();
	void destroyCommandPool();
	void createCommandBuffer();
private:
	VkCommandPool commandPool;
	VkCommandBuffer commandBuffer;

	void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
};
#endif // !ENGINE_VULKAN_COMMAND_POOL