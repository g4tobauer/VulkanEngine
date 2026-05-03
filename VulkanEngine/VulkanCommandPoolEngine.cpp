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
    poolInfo.queueFamilyIndex = pCore->device().queueFamilyIndices().graphicsFamily.value();

    if (vkCreateCommandPool(pCore->device().deviceHandle(), &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create command pool!");
    }
}
void VulkanCommandPoolEngine::destroyCommandPool()
{
	vkDestroyCommandPool(pCore->device().deviceHandle(), commandPool, nullptr);
}
void VulkanCommandPoolEngine::createCommandBuffers()
{
    commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

    if (vkAllocateCommandBuffers(pCore->device().deviceHandle(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
    pCommandBuffers = commandBuffers;
}

VkCommandBuffer VulkanCommandPoolEngine::beginSingleTimeCommands()
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
    if (vkAllocateCommandBuffers(pCore->device().deviceHandle(), &allocInfo, &commandBuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate single-time command buffer!");
    }

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to begin single-time command buffer!");
    }

    return commandBuffer;
}

void VulkanCommandPoolEngine::endSingleTimeCommands(VkCommandBuffer commandBuffer)
{
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to end single-time command buffer!");
    }

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    if (vkQueueSubmit(pCore->device().graphicsQueueHandle(), 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to submit single-time command buffer!");
    }

    vkQueueWaitIdle(pCore->device().graphicsQueueHandle());
    vkFreeCommandBuffers(pCore->device().deviceHandle(), commandPool, 1, &commandBuffer);
}

void VulkanCommandPoolEngine::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, uint32_t frameIndex) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = pCore->swapChain().renderPassHandle();
    renderPassInfo.framebuffer = pCore->swapChain().framebuffers()[imageIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = pCore->swapChain().swapChainExtentValue();

    VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pCore->graphicPipeline().graphicsPipelineHandle());
    VkDescriptorSet frameDescriptorSet = pCore->camera().descriptorSetForFrame(frameIndex);
    vkCmdBindDescriptorSets(
        commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pCore->graphicPipeline().pipelineLayoutHandle(),
        0,
        1,
        &frameDescriptorSet,
        0,
        nullptr);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)pCore->swapChain().swapChainExtentValue().width;
    viewport.height = (float)pCore->swapChain().swapChainExtentValue().height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = pCore->swapChain().swapChainExtentValue();
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    const std::vector<SceneObject>& renderObjects = pCore->scene().renderObjects();
    for (size_t objectIndex = 0; objectIndex < renderObjects.size(); ++objectIndex)
    {
        const SceneObject& object = renderObjects[objectIndex];
        const MeshAssetId meshAssetId = object.meshRenderer.meshAssetId;
        const Material* material = pCore->assets().findMaterialAsset(object.meshRenderer.materialAssetId);
        if (material == nullptr)
        {
            throw std::runtime_error("scene object referenced an invalid material asset!");
        }
        const VkDescriptorSet materialDescriptorSet = pCore->assets().materialDescriptorSet(object.meshRenderer.materialAssetId);

        VkBuffer vertexBuffers[] = { pCore->geometry().vertexBufferHandle(meshAssetId) };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(commandBuffer, pCore->geometry().indexBufferHandle(meshAssetId), 0, VK_INDEX_TYPE_UINT32);
        vkCmdBindDescriptorSets(
            commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pCore->graphicPipeline().pipelineLayoutHandle(),
            1,
            1,
            &materialDescriptorSet,
            0,
            nullptr);

        MeshPushConstants pushConstants{};
        pushConstants.model = pCore->scene().modelMatrixForObject(objectIndex);
        memcpy(pushConstants.baseColor, material->baseColor, sizeof(pushConstants.baseColor));
        vkCmdPushConstants(
            commandBuffer,
            pCore->graphicPipeline().pipelineLayoutHandle(),
            VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            0,
            sizeof(MeshPushConstants),
            &pushConstants);

        vkCmdDrawIndexed(commandBuffer, pCore->geometry().indexCount(meshAssetId), 1, 0, 0, 0);
    }

    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

const std::vector<VkCommandBuffer>& VulkanCommandPoolEngine::commandBuffersView() const
{
	return commandBuffers;
}
#pragma endregion
