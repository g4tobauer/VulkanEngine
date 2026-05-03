#pragma once
#ifndef ENGINE_CORE
#define ENGINE_CORE

#ifndef ENGINE_COMMON_HEADERS
#include "CommonHeaders.h"
#endif // !ENGINE_COMMON_HEADERS

#include "FileStreamEngine.h"
#include "WindowEngine.h"
#include "VulkanInstanceEngine.h"
#include "VulkanDebugCallbackEngine.h"
#include "VulkanDeviceEngine.h"
#include "VulkanSwapChainEngine.h"
#include "VulkanGraphicPipelineEngine.h"
#include "VulkanCommandPoolEngine.h"
#include "VulkanSemaphoresEngine.h"

#define WIDTH 800
#define HEIGHT 600

const int MAX_FRAMES_IN_FLIGHT = 2;

const std::vector<const char*> validationLayers = 
{
	"VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

// Mantemos desativado por padrão para que o projeto rode sem depender do Vulkan SDK
// instalado localmente. Se quiser usar validation layers depois, basta mudar para true
// e garantir que o SDK/Layers estejam presentes na máquina.
constexpr bool enableValidationLayers = false;

class Core
{
public:
	FileStreamEngine* pFileStreamEngine;
	WindowEngine* pWindowEngine;
	VulkanInstanceEngine* pVulkanInstanceEngine;
	VulkanDebugCallbackEngine* pVulkanDebugCallbackEngine;
	VulkanDeviceEngine* pVulkanDeviceEngine;
	VulkanSwapChainEngine* pVulkanSwapChainEngine;
	VulkanGraphicPipelineEngine* pVulkanGraphicPipelineEngine;
	VulkanCommandPoolEngine* pVulkanCommandPoolEngine;
	VulkanSemaphoresEngine* pVulkanSemaphoresEngine;
	
	std::vector<const char*> engineExtentions;

	Core();
	~Core();

	void run();
private:
	void initWindow();
	void initVulkan();
	void mainLoop();
	void cleanup();
	bool checkValidationLayerSupport();
};
#endif // !ENGINE_CORE

