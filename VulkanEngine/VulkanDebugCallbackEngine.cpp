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
	utilsMessengerCallback = NULL;
	utilsMessengerCreateInfo = {};
}

void VulkanDebugCallbackEngine::setupDebugCallback()
{
	if (!enableValidationLayers) return;
	setupUtilsMessengerCreateInfo();
}
void VulkanDebugCallbackEngine::createDebugCallback()
{
	if (!enableValidationLayers) return;
	if (CreateDebugUtilsMessengerEXT(*pCore->instance().pInstance, &utilsMessengerCreateInfo, NULL, &utilsMessengerCallback) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to set up debug callback!");
	}
}
void VulkanDebugCallbackEngine::putExtensions()
{
	pCore->window().putRequiredInstanceExtensions();
	if (enableValidationLayers)
	{
		pCore->instanceExtensions().push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}
}
void VulkanDebugCallbackEngine::destroyDebugCallback()
{
	if (enableValidationLayers) 
	{
		DestroyDebugUtilsMessengerEXT(*pCore->instance().pInstance, utilsMessengerCallback, NULL);
	}
}
#pragma endregion

#pragma region Private
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
	if (func != NULL) {
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
#pragma endregion
