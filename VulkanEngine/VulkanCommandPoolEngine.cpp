#include "VulkanCommandPoolEngine.h"
#include "Core.h"

#pragma region Public

VulkanCommandPoolEngine::VulkanCommandPoolEngine(Core* core)
{
	pCore = core;
}

VulkanCommandPoolEngine::~VulkanCommandPoolEngine()
{
	pCore = NULL;
}

void VulkanCommandPoolEngine::createCommandPool()
{
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = pCore->pVulkanDeviceEngine->indices.graphicsFamily.value();

    if (vkCreateCommandPool(*(pCore->pVulkanDeviceEngine->pDevice), &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create command pool!");
    }
}
void VulkanCommandPoolEngine::destroyCommandPool()
{
	vkDestroyCommandPool(*(pCore->pVulkanDeviceEngine->pDevice), commandPool, nullptr);
}
void VulkanCommandPoolEngine::createCommandBuffer()
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(*(pCore->pVulkanDeviceEngine->pDevice), &allocInfo, &commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

#pragma endregion


#pragma region Private
void VulkanCommandPoolEngine::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }   

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = *(pCore->pVulkanSwapChainEngine->pRenderPass);
    renderPassInfo.framebuffer = pCore->pVulkanSwapChainEngine->pSwapChainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = *(pCore->pVulkanSwapChainEngine->pSwapChainExtent);

    VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *(pCore->pVulkanGraphicPipelineEngine->pGraphicsPipeline));

    
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)(pCore->pVulkanSwapChainEngine->pSwapChainExtent)->width;
    viewport.height = (float)(pCore->pVulkanSwapChainEngine->pSwapChainExtent)->height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = *(pCore->pVulkanSwapChainEngine->pSwapChainExtent);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    vkCmdDraw(commandBuffer, 3, 1, 0, 0);

    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}
#pragma endregion