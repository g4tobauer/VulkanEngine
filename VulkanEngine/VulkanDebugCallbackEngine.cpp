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
	debugReportCallback = NULL;
	utilsMessengerCallback = NULL;
	debugReportCallbackCreateInfo = {};
	utilsMessengerCreateInfo = {};
}
void VulkanDebugCallbackEngine::setupDebugCallback()
{
	if (!enableValidationLayers) return;
	setupDebugReportCallbackCreateInfo();
	setupUtilsMessengerCreateInfo();
	if (CreateDebugReportCallbackEXT(*pCore->pVulkanInstanceEngine->pInstance, &debugReportCallbackCreateInfo, NULL, &debugReportCallback) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to set up debug callback!");
	}
	if (CreateDebugUtilsMessengerEXT(*pCore->pVulkanInstanceEngine->pInstance, &utilsMessengerCreateInfo, NULL, &utilsMessengerCallback) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to set up debug callback!");
	}
}
void VulkanDebugCallbackEngine::putExtensions()
{
	pCore->pWindowEngine->putRequiredInstanceExtensions();
	if (enableValidationLayers)
	{
		pCore->engineExtentions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
		pCore->engineExtentions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}
}
void VulkanDebugCallbackEngine::destroyDebugCallback()
{
	if (enableValidationLayers) 
	{
		DestroyDebugReportCallbackEXT(*pCore->pVulkanInstanceEngine->pInstance, debugReportCallback, NULL);
		DestroyDebugUtilsMessengerEXT(*pCore->pVulkanInstanceEngine->pInstance, utilsMessengerCallback, NULL);
	}
}
#pragma endregion

#pragma region Private
void VulkanDebugCallbackEngine::setupDebugReportCallbackCreateInfo()
{
	debugReportCallbackCreateInfo = {};
	debugReportCallbackCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	debugReportCallbackCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
	debugReportCallbackCreateInfo.pfnCallback = debugCallback;
}
void VulkanDebugCallbackEngine::setupUtilsMessengerCreateInfo()
{
	utilsMessengerCreateInfo = {};
	utilsMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	utilsMessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	utilsMessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	utilsMessengerCreateInfo.pfnUserCallback = messengerCallback;
}

VkResult VulkanDebugCallbackEngine::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}
void VulkanDebugCallbackEngine::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
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
