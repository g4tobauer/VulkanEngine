#include "VulkanSemaphoresEngine.h"
#include "Core.h"

#pragma region Public

VulkanSemaphoresEngine::VulkanSemaphoresEngine(Core* core)
{
    pCore = core;
}

VulkanSemaphoresEngine::~VulkanSemaphoresEngine()
{
    pCore = NULL;
}

void VulkanSemaphoresEngine::createSyncObjects()
{
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    if (vkCreateSemaphore(*(pCore->pVulkanDeviceEngine->pDevice), &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS ||
        vkCreateSemaphore(*(pCore->pVulkanDeviceEngine->pDevice), &semaphoreInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS ||
        vkCreateFence(*(pCore->pVulkanDeviceEngine->pDevice), &fenceInfo, nullptr, &inFlightFence) != VK_SUCCESS) {
        throw std::runtime_error("failed to create synchronization objects for a frame!");
    }
}

void VulkanSemaphoresEngine::drawFrame() {
    vkWaitForFences(*(pCore->pVulkanDeviceEngine->pDevice), 1, &inFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(*(pCore->pVulkanDeviceEngine->pDevice), 1, &inFlightFence);

    uint32_t imageIndex;
    vkAcquireNextImageKHR(*(pCore->pVulkanDeviceEngine->pDevice), *(pCore->pVulkanSwapChainEngine->pSwapChain), UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
        
    vkResetCommandBuffer(*(pCore->pVulkanCommandPoolEngine->pCommandBuffer), /*VkCommandBufferResetFlagBits*/ 0);
    pCore->pVulkanCommandPoolEngine->recordCommandBuffer(*(pCore->pVulkanCommandPoolEngine->pCommandBuffer), imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { imageAvailableSemaphore };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = (pCore->pVulkanCommandPoolEngine->pCommandBuffer);

    VkSemaphore signalSemaphores[] = { renderFinishedSemaphore };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;
        
    if (vkQueueSubmit(*(pCore->pVulkanDeviceEngine->pGraphicsQueue), 1, &submitInfo, inFlightFence) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    
    VkSwapchainKHR swapChains[] = { *(pCore->pVulkanSwapChainEngine->pSwapChain) };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &imageIndex;

    vkQueuePresentKHR(*(pCore->pVulkanDeviceEngine->pPresentQueue), &presentInfo);
}

void VulkanSemaphoresEngine::destroySyncObjects()
{
    vkDestroySemaphore(*(pCore->pVulkanDeviceEngine->pDevice), renderFinishedSemaphore, nullptr);
    vkDestroySemaphore(*(pCore->pVulkanDeviceEngine->pDevice), imageAvailableSemaphore, nullptr);
    vkDestroyFence(*(pCore->pVulkanDeviceEngine->pDevice), inFlightFence, nullptr);
}
#pragma endregion