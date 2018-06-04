#include "VulkanDebugCallbackEngine.h"
#include "Core.h"

#pragma region Public 
VulkanDebugCallbackEngine::VulkanDebugCallbackEngine(Core* core)
{
	pCore = core;
}
VulkanDebugCallbackEngine::~VulkanDebugCallbackEngine()
{
	pCore = NULL;
	callback = NULL;
	createInfo = {};	
}
void VulkanDebugCallbackEngine::setupDebugCallback()
{
	if (!enableValidationLayers) return;
	setupDebugCallbackCreateInfo();
	if (CreateDebugReportCallbackEXT(*pCore->pVulkanInstanceEngine->pInstance, &createInfo, NULL, &callback) != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to set up debug callback!");
	}
}
void VulkanDebugCallbackEngine::putExtensions(std::vector<const char*> *extensions)
{
	extensions->push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
}
void VulkanDebugCallbackEngine::destroyDebugCallback()
{
	if (enableValidationLayers) 
	{
		DestroyDebugReportCallbackEXT(*pCore->pVulkanInstanceEngine->pInstance, callback, NULL);
	}
}
#pragma endregion

#pragma region Private
void VulkanDebugCallbackEngine::setupDebugCallbackCreateInfo()
{
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
	createInfo.pfnCallback = debugCallback;
}
VkResult VulkanDebugCallbackEngine::CreateDebugReportCallbackEXT(VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback) {
	auto func = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
	if (func != NULL) 
	{
		return func(instance, pCreateInfo, pAllocator, pCallback);
	}
	else 
	{
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}
void VulkanDebugCallbackEngine::DestroyDebugReportCallbackEXT(VkInstance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
	if (func != NULL) 
	{
		func(instance, callback, pAllocator);
	}
}
#pragma endregion
