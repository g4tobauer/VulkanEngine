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
	QueueFamilyIndices* pIndices;


	VulkanDeviceEngine(Core* core);
	~VulkanDeviceEngine();

	void pickPhysicalDevice();
	void createLogicalDevice();
	void destroyDevice();
	SwapChainSupportDetails querySwapChainSupport();
private:
	QueueFamilyIndices indices;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkQueue graphicsQueue;
	VkQueue presentQueue;
	VkDevice device;

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	VkPhysicalDeviceFeatures deviceFeatures;
	VkDeviceCreateInfo createInfo;

	void setupQueueCreateInfo();
	void setupDeviceCreateInfo();
	void findQueueFamilies(VkPhysicalDevice device);
	bool isDeviceSuitable(VkPhysicalDevice device);
};
#endif // !ENGINE_VULKAN_DEVICE