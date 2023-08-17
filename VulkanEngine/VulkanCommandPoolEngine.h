#pragma once
#ifndef ENGINE_VULKAN_COMMAND_POOL
#define ENGINE_VULKAN_COMMAND_POOL

#include "BaseEngine.h"

class VulkanCommandPoolEngine : BaseEngine
{
public:
	VkCommandBuffer* pCommandBuffer;

	VulkanCommandPoolEngine(Core* core);
	~VulkanCommandPoolEngine();
	
	void createCommandPool();
	void destroyCommandPool();

	void createCommandBuffer();
	
	void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
private:
	VkCommandPool commandPool;
	VkCommandBuffer commandBuffer;

};
#endif // !ENGINE_VULKAN_COMMAND_POOL