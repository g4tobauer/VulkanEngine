#include "Core.h"

#pragma region Public

Core::Core()
{
	pFileStreamEngine = new FileStreamEngine(this);
	pWindowEngine = new WindowEngine(this);
	pVulkanInstanceEngine = new VulkanInstanceEngine(this);
	pVulkanDebugCallbackEngine = new VulkanDebugCallbackEngine(this);
	pVulkanDeviceEngine = new VulkanDeviceEngine(this);
	pVulkanSwapChainEngine = new VulkanSwapChainEngine(this);
	pVulkanGraphicPipelineEngine = new VulkanGraphicPipelineEngine(this);	
	pVulkanCommandPoolEngine = new VulkanCommandPoolEngine(this);
	pVulkanSemaphoresEngine = new VulkanSemaphoresEngine(this);
}

Core::~Core()
{
	delete pFileStreamEngine;
	pFileStreamEngine = NULL;
	
	delete pVulkanSemaphoresEngine;
	pVulkanSemaphoresEngine = NULL;

	delete pVulkanCommandPoolEngine;
	pVulkanCommandPoolEngine = NULL;

	delete pVulkanGraphicPipelineEngine;
	pVulkanGraphicPipelineEngine = NULL;

	delete pVulkanSwapChainEngine;
	pVulkanSwapChainEngine = NULL;

	delete pVulkanDeviceEngine;
	pVulkanDeviceEngine = NULL;

	delete pVulkanDebugCallbackEngine;
	pVulkanDebugCallbackEngine = NULL;

	delete pVulkanInstanceEngine;
	pVulkanInstanceEngine = NULL;

	delete pWindowEngine;
	pWindowEngine = NULL;

	engineExtentions.clear();
}

void Core::run()
{
	initWindow();
	initVulkan();
	mainLoop();
	cleanup();
}

#pragma endregion

#pragma region Private

void Core::initWindow()
{
	pWindowEngine->createWindow();
}

void Core::initVulkan()
{
	if (enableValidationLayers && !checkValidationLayerSupport()) 
	{
		throw std::runtime_error("validation layers requested, but not available!");
	}
	pVulkanInstanceEngine->createInstance();
	pWindowEngine->createSurface();
	pVulkanDeviceEngine->pickPhysicalDevice();
	pVulkanDeviceEngine->createLogicalDevice();
	pVulkanSwapChainEngine->createSwapChain();
	pVulkanSwapChainEngine->createImageViews();
	pVulkanSwapChainEngine->createRenderPass();
	pVulkanGraphicPipelineEngine->createGraphicsPipeline();
	pVulkanSwapChainEngine->createFramebuffers();
	pVulkanCommandPoolEngine->createCommandPool();
	pVulkanCommandPoolEngine->createCommandBuffers();
	pVulkanSemaphoresEngine->createSyncObjects();
}

void Core::mainLoop()
{
	while (pWindowEngine->isOpen())
	{
		pVulkanSemaphoresEngine->drawFrame();
	}
	vkDeviceWaitIdle(*(pVulkanDeviceEngine->pDevice));
}

void Core::cleanup()
{
	pVulkanSwapChainEngine->cleanupSwapChain();
	pVulkanGraphicPipelineEngine->destroyGraphicsPipeline();
	pVulkanSwapChainEngine->destroyRenderPass();
	pVulkanSemaphoresEngine->destroySyncObjects();
	pVulkanCommandPoolEngine->destroyCommandPool();
	pVulkanDeviceEngine->destroyDevice();
	pVulkanDebugCallbackEngine->destroyDebugCallback();
	pWindowEngine->destroySurface();
	pVulkanInstanceEngine->destroyInstance();
	pWindowEngine->destroyWindow();
}

bool Core::checkValidationLayerSupport()
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : validationLayers)
	{
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers)
		{
			if (strcmp(layerName, layerProperties.layerName) == 0)
			{
				layerFound = true;
				break;
			}
		}
		if (!layerFound)
		{
			return false;
		}
	}
	return true;
}
#pragma endregion
