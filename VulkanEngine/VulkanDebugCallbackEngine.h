#pragma once
#ifndef ENGINE_VULKAN_DEBUG_CALLBACK
#define ENGINE_VULKAN_DEBUG_CALLBACK

#include "BaseEngine.h"

static VKAPI_ATTR VkBool32 VKAPI_CALL messengerCallback(
	[[maybe_unused]] VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	[[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	[[maybe_unused]] void* pUserData) {
	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

	return VK_FALSE;
}

class VulkanDebugCallbackEngine : BaseEngine
{
public:
	VkDebugUtilsMessengerCreateInfoEXT utilsMessengerCreateInfo;

	VulkanDebugCallbackEngine(Core* core);
	~VulkanDebugCallbackEngine();

	void setupDebugCallback();
	void createDebugCallback();
	void putExtensions();
	void destroyDebugCallback();
private:
	VkDebugUtilsMessengerEXT utilsMessengerCallback;

	void setupUtilsMessengerCreateInfo();

	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
};
#endif // !ENGINE_VULKAN_DEBUG_CALLBACK
