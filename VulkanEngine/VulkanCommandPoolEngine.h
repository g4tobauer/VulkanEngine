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

	void createCommandBuffers();
	
	void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, uint32_t frameIndex);
	const std::vector<VkCommandBuffer>& commandBuffersView() const;
private:
	std::vector<VkCommandBuffer> pCommandBuffers;
	VkCommandPool commandPool;
	std::vector<VkCommandBuffer> commandBuffers;
};
#endif // !ENGINE_VULKAN_COMMAND_POOL
