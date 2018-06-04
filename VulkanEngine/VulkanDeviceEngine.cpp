#include "VulkanDeviceEngine.h"
#include "Core.h"

#pragma region Public

VulkanDeviceEngine::VulkanDeviceEngine(Core* core)
{
	pCore = core;
}

VulkanDeviceEngine::~VulkanDeviceEngine()
{
	queueCreateInfo = {};
	createInfo = {};
	deviceFeatures = {};
	indices = {};

	pGraphicsQueue = NULL;
	graphicsQueue = NULL;

	pDevice = NULL;
	device = NULL;
	
	pPhysicalDevice = NULL;
	physicalDevice = VK_NULL_HANDLE;

	pCore = NULL;
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
	vkGetDeviceQueue(device, indices.graphicsFamily, 0, &graphicsQueue);
	pDevice = &device;
	pGraphicsQueue = &graphicsQueue;
}

void VulkanDeviceEngine::destroyDevice()
{
	vkDestroyDevice(device, NULL);
}
#pragma endregion

#pragma region Private

void VulkanDeviceEngine::setupQueueCreateInfo()
{
	queueCreateInfo = {};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = indices.graphicsFamily;
	queueCreateInfo.queueCount = 1;
	queueCreateInfo.pQueuePriorities = &indices.queuePriority;
}

void VulkanDeviceEngine::setupDeviceCreateInfo()
{
	deviceFeatures = {};
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = &queueCreateInfo;
	createInfo.queueCreateInfoCount = 1;
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = 0;

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
		if (indices.isComplete()) 
		{
			break;
		}
		i++;
	}
}

#pragma endregion