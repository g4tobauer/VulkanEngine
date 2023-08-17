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
	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore;
	VkFence inFlightFence;
};
#endif // !ENGINE_VULKAN_SEMAPHORES