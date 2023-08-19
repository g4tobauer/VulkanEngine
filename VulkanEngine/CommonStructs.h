#pragma once
#ifndef ENGINE_COMMON_STRUCTS
#define ENGINE_COMMON_STRUCTS

#ifndef ENGINE_COMMON_HEADERS
#include "CommonHeaders.h"
#endif // !#ifndef ENGINE_COMMON_HEADERS


struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};


struct QueueFamilyIndices
{
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;
	float queuePriority = 1.0f;
	//int graphicsFamily = -1;
	//int presentFamily = -1;

	bool isComplete()
	{
		return graphicsFamily.has_value() && presentFamily.has_value();
		//return graphicsFamily.has_value() && presentFamily >= 0;
	}
};

#endif // !ENGINE_COMMON_STRUCTS