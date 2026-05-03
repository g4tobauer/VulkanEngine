#pragma once
#ifndef ENGINE_CAMERA
#define ENGINE_CAMERA

#include "BaseEngine.h"
#include "VulkanBuffer.h"

class CameraEngine : BaseEngine
{
public:
    explicit CameraEngine(Core* core);
    ~CameraEngine();

    void initialize();
    void shutdown();
    void updateUniformBuffer(uint32_t currentFrame);

    VkDescriptorSetLayout descriptorSetLayoutHandle() const;
    VkDescriptorSet descriptorSetForFrame(uint32_t frameIndex) const;

private:
    VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
    std::vector<VulkanBuffer> uniformBuffers;
    std::vector<VkDescriptorSet> descriptorSets;

    void createDescriptorSetLayout();
    void createUniformBuffers();
    void createDescriptorPool();
    void createDescriptorSets();
};

#endif // !ENGINE_CAMERA
