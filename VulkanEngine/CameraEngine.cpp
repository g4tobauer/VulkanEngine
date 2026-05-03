#include "CameraEngine.h"
#include "Core.h"

CameraEngine::CameraEngine(Core* core)
{
    pCore = core;
}

CameraEngine::~CameraEngine()
{
    pCore = NULL;
}

void CameraEngine::initialize()
{
    createDescriptorSetLayout();
    createUniformBuffers();
    createDescriptorPool();
    createDescriptorSets();
}

void CameraEngine::shutdown()
{
    VkDevice device = pCore->device().deviceHandle();

    for (size_t i = 0; i < uniformBuffers.size(); ++i)
    {
        uniformBuffers[i].destroy(*pCore);
    }

    uniformBuffers.clear();
    descriptorSets.clear();

    if (descriptorPool != VK_NULL_HANDLE)
    {
        vkDestroyDescriptorPool(device, descriptorPool, nullptr);
        descriptorPool = VK_NULL_HANDLE;
    }

    if (descriptorSetLayout != VK_NULL_HANDLE)
    {
        vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
        descriptorSetLayout = VK_NULL_HANDLE;
    }
}

void CameraEngine::updateUniformBuffer(uint32_t currentFrame)
{
    UniformBufferObject ubo{};
    const VkExtent2D extent = pCore->swapChain().swapChainExtentValue();
    const float aspectRatio = extent.height == 0
        ? 1.0f
        : static_cast<float>(extent.width) / static_cast<float>(extent.height);

    ubo.view = Mat4::translation(0.0f, 0.0f, -2.5f);
    ubo.projection = Mat4::perspective(3.14159265f / 3.0f, aspectRatio, 0.1f, 10.0f);
    ubo.projection.elements[5] *= -1.0f;
    ubo.viewProjection = multiply(ubo.projection, ubo.view);
    ubo.lightDirection[0] = -0.45f;
    ubo.lightDirection[1] = -0.8f;
    ubo.lightDirection[2] = -0.35f;
    ubo.lightDirection[3] = 0.0f;
    ubo.lightColor[0] = 1.0f;
    ubo.lightColor[1] = 0.98f;
    ubo.lightColor[2] = 0.92f;
    ubo.lightColor[3] = 1.0f;
    ubo.ambientColor[0] = 0.22f;
    ubo.ambientColor[1] = 0.24f;
    ubo.ambientColor[2] = 0.30f;
    ubo.ambientColor[3] = 1.0f;

    uniformBuffers[currentFrame].write(&ubo, sizeof(ubo));
}

VkDescriptorSetLayout CameraEngine::descriptorSetLayoutHandle() const
{
    return descriptorSetLayout;
}

VkDescriptorSet CameraEngine::descriptorSetForFrame(uint32_t frameIndex) const
{
    return descriptorSets[frameIndex];
}

void CameraEngine::createDescriptorSetLayout()
{
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &uboLayoutBinding;

    if (vkCreateDescriptorSetLayout(pCore->device().deviceHandle(), &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

void CameraEngine::createUniformBuffers()
{
    const VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        uniformBuffers[i].create(
            *pCore,
            bufferSize,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        uniformBuffers[i].map(*pCore);
    }
}

void CameraEngine::createDescriptorPool()
{
    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = MAX_FRAMES_IN_FLIGHT;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = MAX_FRAMES_IN_FLIGHT;

    if (vkCreateDescriptorPool(pCore->device().deviceHandle(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

void CameraEngine::createDescriptorSets()
{
    std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = MAX_FRAMES_IN_FLIGHT;
    allocInfo.pSetLayouts = layouts.data();

    descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
    if (vkAllocateDescriptorSets(pCore->device().deviceHandle(), &allocInfo, descriptorSets.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = uniformBuffers[i].handle();
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);

        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = descriptorSets[i];
        descriptorWrite.dstBinding = 0;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(pCore->device().deviceHandle(), 1, &descriptorWrite, 0, nullptr);
    }
}
