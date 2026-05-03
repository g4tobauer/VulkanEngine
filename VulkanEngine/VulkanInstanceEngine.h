#pragma once
#ifndef ENGINE_VULKAN_INSTANCE
#define ENGINE_VULKAN_INSTANCE

#include "BaseEngine.h"

class VulkanInstanceEngine : BaseEngine
{
public:
	VulkanInstanceEngine(Core* core);
	~VulkanInstanceEngine();

	void createInstance();
	void destroyInstance();
	VkInstance instanceHandle() const;
	VkInstance* instancePtr();

private:
	VkInstance* pInstance;
	VkInstance instance;
	VkApplicationInfo appInfo;
	VkInstanceCreateInfo createInfo;

	void setupAppInfo();
	void setupCreateInfo();
};
#endif // !ENGINE_VULKAN_INSTANCE
