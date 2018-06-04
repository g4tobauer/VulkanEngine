#pragma once
#ifndef ENGINE_VULKAN_DEVICE
#define ENGINE_VULKAN_DEVICE

#include "BaseEngine.h"

class VulkanDeviceEngine : BaseEngine
{
public:
	VkPhysicalDevice* pPhysicalDevice;
	VkDevice* pDevice;
	VkQueue* pGraphicsQueue;
	VkQueue* pPresentQueue;

	VulkanDeviceEngine(Core* core);
	~VulkanDeviceEngine();

	void pickPhysicalDevice();
	void createLogicalDevice();
	void destroyDevice();

private:
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkQueue graphicsQueue;
	VkQueue presentQueue;
	VkDevice device;

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	VkPhysicalDeviceFeatures deviceFeatures;
	VkDeviceCreateInfo createInfo;

	struct QueueFamilyIndices
	{
		float queuePriority = 1.0f;
		int graphicsFamily = -1;
		int presentFamily = -1;

		bool isComplete()
		{
			return graphicsFamily >= 0 && presentFamily >= 0;
		}
	} indices;

	void setupQueueCreateInfo();
	void setupDeviceCreateInfo();
	void findQueueFamilies(VkPhysicalDevice device);
	bool isDeviceSuitable(VkPhysicalDevice device);
};
#endif // !ENGINE_VULKAN_DEVICE