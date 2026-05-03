#pragma once
#ifndef ENGINE_VULKAN_CONTEXT
#define ENGINE_VULKAN_CONTEXT

#include "Core.h"

class VulkanContext
{
public:
	explicit VulkanContext(Core& core);

	void initialize();
	void shutdown();

private:
	Core& core_;
};

#endif // !ENGINE_VULKAN_CONTEXT
