#include "Core.h"

Core::Core()
	: fileStreamEngine_(std::make_unique<FileStreamEngine>(this)),
	  windowEngine_(std::make_unique<WindowEngine>(this)),
	  vulkanInstanceEngine_(std::make_unique<VulkanInstanceEngine>(this)),
	  vulkanDebugCallbackEngine_(std::make_unique<VulkanDebugCallbackEngine>(this)),
	  vulkanDeviceEngine_(std::make_unique<VulkanDeviceEngine>(this)),
	  vulkanSwapChainEngine_(std::make_unique<VulkanSwapChainEngine>(this)),
	  vulkanGraphicPipelineEngine_(std::make_unique<VulkanGraphicPipelineEngine>(this)),
	  geometryEngine_(std::make_unique<GeometryEngine>(this)),
	  sceneEngine_(std::make_unique<SceneEngine>(this)),
	  cameraEngine_(std::make_unique<CameraEngine>(this)),
	  vulkanCommandPoolEngine_(std::make_unique<VulkanCommandPoolEngine>(this)),
	  vulkanSemaphoresEngine_(std::make_unique<VulkanSemaphoresEngine>(this))
{
}

Core::~Core() = default;

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

FileStreamEngine& Core::fileStream() const
{
	return *fileStreamEngine_;
}

WindowEngine& Core::window() const
{
	return *windowEngine_;
}

VulkanInstanceEngine& Core::instance() const
{
	return *vulkanInstanceEngine_;
}

VulkanDebugCallbackEngine& Core::debugCallback() const
{
	return *vulkanDebugCallbackEngine_;
}

VulkanDeviceEngine& Core::device() const
{
	return *vulkanDeviceEngine_;
}

VulkanSwapChainEngine& Core::swapChain() const
{
	return *vulkanSwapChainEngine_;
}

VulkanGraphicPipelineEngine& Core::graphicPipeline() const
{
	return *vulkanGraphicPipelineEngine_;
}

GeometryEngine& Core::geometry() const
{
	return *geometryEngine_;
}

SceneEngine& Core::scene() const
{
	return *sceneEngine_;
}

CameraEngine& Core::camera() const
{
	return *cameraEngine_;
}

VulkanCommandPoolEngine& Core::commandPool() const
{
	return *vulkanCommandPoolEngine_;
}

VulkanSemaphoresEngine& Core::semaphores() const
{
	return *vulkanSemaphoresEngine_;
}

std::vector<const char*>& Core::instanceExtensions()
{
	return engineExtentions_;
}

const std::vector<const char*>& Core::instanceExtensions() const
{
	return engineExtentions_;
}
