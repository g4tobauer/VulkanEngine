#pragma once
#ifndef ENGINE_VULKAN_SWAPCHAIN
#define ENGINE_VULKAN_SWAPCHAIN

#include "BaseEngine.h"

class VulkanSwapChainEngine : BaseEngine
{
public:
    VkFormat* pSwapChainImageFormat;
    VkRenderPass* pRenderPass;
    VkExtent2D* pSwapChainExtent;
    std::vector<VkFramebuffer> pSwapChainFramebuffers;

	VulkanSwapChainEngine(Core* core);
	~VulkanSwapChainEngine();

    void createSwapChain(); 
    void destroySwapChain();

    void createRenderPass();
    void destroyRenderPass();

    void createFramebuffers();
    void destroyFramebuffers();
private:

    VkSwapchainKHR swapChain;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    VkRenderPass renderPass;
    std::vector<VkFramebuffer> swapChainFramebuffers;
    std::vector<VkImage> swapChainImages;
    std::vector<VkImageView> swapChainImageViews;

    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    void createImageViews();
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
};

#endif // !ENGINE_VULKAN_SWAPCHAIN