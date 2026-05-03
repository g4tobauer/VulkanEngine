#include "Application.h"

Application::Application()
	: core_(),
	  renderer_(core_)
{
}

void Application::run()
{
	initWindow();
	initVulkan();
	mainLoop();
	cleanup();
}

void Application::initWindow()
{
	core_.window().createWindow();
}

void Application::initVulkan()
{
	if (volkInitialize() != VK_SUCCESS)
	{
		throw std::runtime_error("failed to initialize volk!");
	}

	if constexpr (enableValidationLayers)
	{
		if (!core_.checkValidationLayerSupport())
		{
			throw std::runtime_error("validation layers requested, but not available!");
		}
	}

	core_.instance().createInstance();
	volkLoadInstance(*core_.instance().pInstance);
	core_.window().createSurface();
	core_.device().pickPhysicalDevice();
	core_.device().createLogicalDevice();
	volkLoadDevice(*core_.device().pDevice);
	renderer_.initialize();
}

void Application::mainLoop()
{
	while (core_.window().isOpen())
	{
		renderer_.drawFrame();
	}

	renderer_.waitIdle();
}

void Application::cleanup()
{
	renderer_.shutdown();
	core_.device().destroyDevice();
	core_.debugCallback().destroyDebugCallback();
	core_.window().destroySurface();
	core_.instance().destroyInstance();
	core_.window().destroyWindow();
}
