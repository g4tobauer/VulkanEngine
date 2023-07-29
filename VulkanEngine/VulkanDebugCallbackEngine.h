#pragma once
#ifndef ENGINE_VULKAN_DEBUG_CALLBACK
#define ENGINE_VULKAN_DEBUG_CALLBACK

#include "BaseEngine.h"

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t obj, size_t location, int32_t code, const char* layerPrefix, const char* msg, void* userData) 
{
	std::cerr << "validation layer: " << msg << std::endl;
	return VK_FALSE;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL messengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

	return VK_FALSE;
}

class VulkanDebugCallbackEngine : BaseEngine
{
public:
	VkDebugUtilsMessengerCreateInfoEXT utilsMessengerCreateInfo;
	VkDebugReportCallbackCreateInfoEXT debugReportCallbackCreateInfo;

	VulkanDebugCallbackEngine(Core* core);
	~VulkanDebugCallbackEngine();

	void setupDebugCallback();
	void createDebugCallback();
	void putExtensions();
	void destroyDebugCallback();
private:
	VkDebugReportCallbackEXT debugReportCallback;
	VkDebugUtilsMessengerEXT utilsMessengerCallback;

	void setupUtilsMessengerCreateInfo();
	void setupDebugReportCallbackCreateInfo();

	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

	VkResult CreateDebugReportCallbackEXT(VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback);
	void DestroyDebugReportCallbackEXT(VkInstance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* pAllocator);
};
#endif // !ENGINE_VULKAN_DEBUG_CALLBACK