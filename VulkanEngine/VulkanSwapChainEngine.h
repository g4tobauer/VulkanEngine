#pragma once
#ifndef ENGINE_VULKAN_SWAPCHAIN
#define ENGINE_VULKAN_SWAPCHAIN

#include "BaseEngine.h"

class VulkanSwapChainEngine : BaseEngine
{
public:
	VulkanSwapChainEngine(Core* core);
	~VulkanSwapChainEngine();

    void createSwapChain();
    void recreateSwapChain();
    void cleanupSwapChain();
    
    void createRenderPass();
    void destroyRenderPass();

    void createFramebuffers();

    void createImageViews();
    VkSwapchainKHR swapChainHandle() const;
    VkFormat swapChainImageFormatValue() const;
    VkRenderPass renderPassHandle() const;
    const VkExtent2D& swapChainExtentValue() const;
    const std::vector<VkFramebuffer>& framebuffers() const;
private:
    VkSwapchainKHR* pSwapChain;
    VkFormat* pSwapChainImageFormat;
    VkRenderPass* pRenderPass;
    VkExtent2D* pSwapChainExtent;
    std::vector<VkFramebuffer> pSwapChainFramebuffers;

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
