#pragma once
#ifndef ENGINE_VULKAN_INSTANCE
#define ENGINE_VULKAN_INSTANCE

#include "BaseEngine.h"

class VulkanInstanceEngine : BaseEngine
{
public:
	VkInstance* pInstance;

	VulkanInstanceEngine(Core* core);
	~VulkanInstanceEngine();

	void createInstance();
	void destroyInstance();

private:
	VkInstance instance;
	VkApplicationInfo appInfo;
	VkInstanceCreateInfo createInfo;

	void setupAppInfo();
	void setupCreateInfo();
};
#endif // !ENGINE_VULKAN_INSTANCE