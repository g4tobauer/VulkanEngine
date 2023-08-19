#include "VulkanDeviceEngine.h"
#include "Core.h"

#pragma region Public

VulkanDeviceEngine::VulkanDeviceEngine(Core* core)
{
	pCore = core;
}

VulkanDeviceEngine::~VulkanDeviceEngine()
{
	createInfo = {};
	deviceFeatures = {};
	indices = {};

	pIndices = NULL;
	pGraphicsQueue = NULL;
	graphicsQueue = NULL;

	pDevice = NULL;
	device = NULL;
	
	pPhysicalDevice = NULL;
	physicalDevice = VK_NULL_HANDLE;

	pCore = NULL;

	queueCreateInfos.clear();
}

void VulkanDeviceEngine::pickPhysicalDevice()
{	
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(*pCore->pVulkanInstanceEngine->pInstance, &deviceCount, NULL);

	if (deviceCount == 0) 
	{
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(*pCore->pVulkanInstanceEngine->pInstance, &deviceCount, devices.data());

	for (const auto& device : devices) 
	{
		if (isDeviceSuitable(device)) 
		{
			physicalDevice = device;
			break;
		}
	}
	if (physicalDevice == VK_NULL_HANDLE) 
	{
		throw std::runtime_error("failed to find a suitable GPU!");
	}
	pPhysicalDevice = &physicalDevice;
}

void VulkanDeviceEngine::createLogicalDevice()
{
	setupQueueCreateInfo();
	setupDeviceCreateInfo();
	if (vkCreateDevice(physicalDevice, &createInfo, NULL, &device) != VK_SUCCESS) {
		throw std::runtime_error("failed to create logical device!");
	}
	vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
	vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);

	pDevice = &device;
	pGraphicsQueue = &graphicsQueue;
	pPresentQueue = &presentQueue;
}

void VulkanDeviceEngine::destroyDevice()
{
	vkDestroyDevice(device, NULL);
}

SwapChainSupportDetails VulkanDeviceEngine::querySwapChainSupport()
{
	VkPhysicalDevice device = physicalDevice;
	VkSurfaceKHR surface = *(pCore->pWindowEngine->pSurface);
	SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}
#pragma endregion

#pragma region Private

void VulkanDeviceEngine::setupQueueCreateInfo()
{
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	for (uint32_t queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &indices.queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}
}

void VulkanDeviceEngine::setupDeviceCreateInfo()
{
	deviceFeatures = {};
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	if (enableValidationLayers) 
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else 
	{
		createInfo.enabledLayerCount = 0;
	}
}

bool VulkanDeviceEngine::isDeviceSuitable(VkPhysicalDevice device) 
{
	/*
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);

	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
	*/

	findQueueFamilies(device);
	return indices.isComplete();
}

void VulkanDeviceEngine::findQueueFamilies(VkPhysicalDevice device)
{
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, NULL);
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
	int i = 0;
	for (const auto& queueFamily : queueFamilies)
	{
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.graphicsFamily = i;
		}
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, *pCore->pWindowEngine->pSurface, &presentSupport);
		if (queueFamily.queueCount > 0 && presentSupport)
		{
			indices.presentFamily = i;
		}
		if (indices.isComplete())
		{
			break;
		}
		i++;
	}
	pIndices = &indices;
}

#pragma endregion