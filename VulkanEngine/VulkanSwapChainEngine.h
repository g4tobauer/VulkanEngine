#pragma once
#ifndef ENGINE_VULKAN_SWAPCHAIN
#define ENGINE_VULKAN_SWAPCHAIN

#include "BaseEngine.h"

class VulkanSwapChainEngine : BaseEngine
{
public:
    VkSwapchainKHR* pSwapChain;
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

    void createImageViews();
    void destroyImageViews();
private:

    VkSwapchainKHR swapChain;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    VkRenderPass renderPass;
    std::vector<VkFramebuffer> swapChainFramebuffers;
    std::vector<VkImage> swapChainImages;
    std::vector<VkImageView> swapChainImageViews;

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
};

#endif // !ENGINE_VULKAN_SWAPCHAIN