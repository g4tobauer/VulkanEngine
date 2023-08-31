#pragma once
#ifndef ENGINE_VULKAN_COMMAND_POOL
#define ENGINE_VULKAN_COMMAND_POOL

#include "BaseEngine.h"

class VulkanCommandPoolEngine : BaseEngine
{
public:
	std::vector<VkCommandBuffer> pCommandBuffers;

	VulkanCommandPoolEngine(Core* core);
	~VulkanCommandPoolEngine();
	
	void createCommandPool();
	void destroyCommandPool();

	void createCommandBuffers();
	
	void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
private:
	VkCommandPool commandPool;
	std::vector<VkCommandBuffer> commandBuffers;
};
#endif // !ENGINE_VULKAN_COMMAND_POOL