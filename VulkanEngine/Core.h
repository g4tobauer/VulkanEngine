#pragma once
#ifndef ENGINE_CORE
#define ENGINE_CORE

#ifndef ENGINE_COMMON_HEADERS
#include "CommonHeaders.h"
#endif // !ENGINE_COMMON_HEADERS

#include "WindowEngine.h"
#include "VulkanInstanceEngine.h"
#include "VulkanDebugCallbackEngine.h"
#include "VulkanDeviceEngine.h"

#define WIDTH 800
#define HEIGHT 600

const std::vector<const char*> validationLayers = 
{
	"VK_LAYER_KHRONOS_validation",
	"VK_LAYER_LUNARG_standard_validation"
};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

class Core
{
public:
	WindowEngine* pWindowEngine;
	VulkanInstanceEngine* pVulkanInstanceEngine;
	VulkanDebugCallbackEngine* pVulkanDebugCallbackEngine;
	VulkanDeviceEngine* pVulkanDeviceEngine;
	std::vector<const char*> engineExtentions;

	Core();
	~Core();

	void run();
private:
	void initWindow();
	void initVulkan();
	void mainLoop();
	void cleanup();
	void putRequiredExtensions();
	bool checkValidationLayerSupport();
};
#endif // !ENGINE_CORE

