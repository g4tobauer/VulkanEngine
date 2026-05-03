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
#include "GeometryEngine.h"
#include "SceneEngine.h"
#include "CameraEngine.h"
#include "VulkanCommandPoolEngine.h"
#include "VulkanSemaphoresEngine.h"
#include <memory>

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
	Core();
	~Core();

	bool checkValidationLayerSupport();

	FileStreamEngine& fileStream() const;
	WindowEngine& window() const;
	VulkanInstanceEngine& instance() const;
	VulkanDebugCallbackEngine& debugCallback() const;
	VulkanDeviceEngine& device() const;
	VulkanSwapChainEngine& swapChain() const;
	VulkanGraphicPipelineEngine& graphicPipeline() const;
	GeometryEngine& geometry() const;
	SceneEngine& scene() const;
	CameraEngine& camera() const;
	VulkanCommandPoolEngine& commandPool() const;
	VulkanSemaphoresEngine& semaphores() const;

	std::vector<const char*>& instanceExtensions();
	const std::vector<const char*>& instanceExtensions() const;

private:
	std::unique_ptr<FileStreamEngine> fileStreamEngine_;
	std::unique_ptr<WindowEngine> windowEngine_;
	std::unique_ptr<VulkanInstanceEngine> vulkanInstanceEngine_;
	std::unique_ptr<VulkanDebugCallbackEngine> vulkanDebugCallbackEngine_;
	std::unique_ptr<VulkanDeviceEngine> vulkanDeviceEngine_;
	std::unique_ptr<VulkanSwapChainEngine> vulkanSwapChainEngine_;
	std::unique_ptr<VulkanGraphicPipelineEngine> vulkanGraphicPipelineEngine_;
	std::unique_ptr<GeometryEngine> geometryEngine_;
	std::unique_ptr<SceneEngine> sceneEngine_;
	std::unique_ptr<CameraEngine> cameraEngine_;
	std::unique_ptr<VulkanCommandPoolEngine> vulkanCommandPoolEngine_;
	std::unique_ptr<VulkanSemaphoresEngine> vulkanSemaphoresEngine_;
	std::vector<const char*> engineExtentions_;
};
#endif // !ENGINE_CORE

