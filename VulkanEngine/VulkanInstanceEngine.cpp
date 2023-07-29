#include "VulkanInstanceEngine.h"
#include "Core.h"

#pragma region Public

VulkanInstanceEngine::VulkanInstanceEngine(Core* core)
{
	pCore = core;
}

VulkanInstanceEngine::~VulkanInstanceEngine()
{
	appInfo = {};
	createInfo = {};
	pInstance = NULL;
	instance = NULL;
	pCore = NULL;
}

void VulkanInstanceEngine::createInstance()
{
	setupAppInfo();
	setupCreateInfo();
	if (vkCreateInstance(&createInfo, NULL, &instance) != VK_SUCCESS) {
		throw std::runtime_error("failed to create instance!");
	}
	pInstance = &instance;
	pCore->pVulkanDebugCallbackEngine->createDebugCallback();
}

void VulkanInstanceEngine::destroyInstance()
{
	vkDestroyInstance(instance, NULL);
}

#pragma endregion

#pragma region Private

void VulkanInstanceEngine::setupAppInfo()
{
	appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Hello Triangle";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;
}
void VulkanInstanceEngine::setupCreateInfo()
{
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	pCore->pVulkanDebugCallbackEngine->putExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(pCore->engineExtentions.size());
	createInfo.ppEnabledExtensionNames = pCore->engineExtentions.data();
	if (enableValidationLayers) 
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();

		pCore->pVulkanDebugCallbackEngine->setupDebugCallback();
		createInfo.pNext = &(pCore->pVulkanDebugCallbackEngine->utilsMessengerCreateInfo);
	}else 
	{
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = NULL;
	}
}

#pragma endregion