#include "Core.h"

#pragma region Public

Core::Core()
{
	pWindowEngine = new WindowEngine(this);
	pVulkanInstanceEngine = new VulkanInstanceEngine(this);
	pVulkanDebugCallbackEngine = new VulkanDebugCallbackEngine(this);
	pVulkanDeviceEngine = new VulkanDeviceEngine(this);
}

Core::~Core()
{
	delete pVulkanDeviceEngine;
	pVulkanDeviceEngine = NULL;

	delete pVulkanDebugCallbackEngine;
	pVulkanDebugCallbackEngine = NULL;

	delete pVulkanInstanceEngine;
	pVulkanInstanceEngine = NULL;

	delete pWindowEngine;
	pWindowEngine = NULL;

	engineExtentions.clear();
}

void Core::run()
{
	initWindow();
	initVulkan();
	mainLoop();
	cleanup();
}

void Core::putRequiredExtensions()
{	
	pWindowEngine->putRequiredInstanceExtensions(&engineExtentions);
	if (enableValidationLayers) 
	{
		pVulkanDebugCallbackEngine->putExtensions(&engineExtentions);
	}
}
#pragma endregion

#pragma region Private

void Core::initWindow()
{
	pWindowEngine->createWindow();
}

void Core::initVulkan()
{
	if (enableValidationLayers && !checkValidationLayerSupport()) 
	{
		throw std::runtime_error("validation layers requested, but not available!");
	}
	pVulkanInstanceEngine->createInstance();
	pVulkanDebugCallbackEngine->setupDebugCallback();
	pVulkanDeviceEngine->pickPhysicalDevice();
	pVulkanDeviceEngine->createLogicalDevice();
}

void Core::mainLoop()
{
	while (pWindowEngine->isOpen())
	{

	}
}

void Core::cleanup()
{
	pVulkanDeviceEngine->destroyDevice();
	pVulkanDebugCallbackEngine->destroyDebugCallback();
	pVulkanInstanceEngine->destroyInstance();
	pWindowEngine->destroyWindow();
}

bool Core::checkValidationLayerSupport()
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : validationLayers)
	{
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers)
		{
			if (strcmp(layerName, layerProperties.layerName) == 0)
			{
				layerFound = true;
				break;
			}
		}
		if (!layerFound)
		{
			return false;
		}
	}
	putRequiredExtensions();
	return true;
}
#pragma endregion
