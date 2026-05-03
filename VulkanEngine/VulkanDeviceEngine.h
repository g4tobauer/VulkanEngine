#pragma once
#ifndef ENGINE_VULKAN_DEVICE
#define ENGINE_VULKAN_DEVICE

#include "BaseEngine.h"

class VulkanDeviceEngine : BaseEngine
{
public:
	VulkanDeviceEngine(Core* core);
	~VulkanDeviceEngine();

	void pickPhysicalDevice();
	void createLogicalDevice();
	void destroyDevice();
	SwapChainSupportDetails querySwapChainSupport();
	VkPhysicalDevice physicalDeviceHandle() const;
	VkDevice deviceHandle() const;
	VkQueue graphicsQueueHandle() const;
	VkQueue presentQueueHandle() const;
	const QueueFamilyIndices& queueFamilyIndices() const;
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;
private:
	VkPhysicalDevice* pPhysicalDevice;
	VkDevice* pDevice;
	VkQueue* pGraphicsQueue;
	VkQueue* pPresentQueue;
	QueueFamilyIndices* pIndices;
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
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
};
#endif // !ENGINE_VULKAN_DEVICE
