#include "VulkanContext.h"

VulkanContext::VulkanContext(Core& core)
	: core_(core)
{
}

void VulkanContext::initialize()
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
	volkLoadInstance(core_.instance().instanceHandle());
	core_.window().createSurface();
	core_.device().pickPhysicalDevice();
	core_.device().createLogicalDevice();
	volkLoadDevice(core_.device().deviceHandle());
}

void VulkanContext::shutdown()
{
	core_.device().destroyDevice();
	core_.debugCallback().destroyDebugCallback();
	core_.window().destroySurface();
	core_.instance().destroyInstance();
}
