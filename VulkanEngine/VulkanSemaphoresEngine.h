#pragma once
#ifndef ENGINE_VULKAN_SEMAPHORES
#define ENGINE_VULKAN_SEMAPHORES

#include "BaseEngine.h"

class VulkanSemaphoresEngine : BaseEngine
{
public:
	VulkanSemaphoresEngine(Core* core);
	~VulkanSemaphoresEngine();
	
	void createSyncObjects();
	void destroySyncObjects();

	void drawFrame();
private:
	uint32_t currentFrame = 0;
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
};
#endif // !ENGINE_VULKAN_SEMAPHORES