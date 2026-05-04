#pragma once
#ifndef ENGINE_CAMERA
#define ENGINE_CAMERA

#include "BaseEngine.h"
#include "VulkanBuffer.h"

class CameraEngine : BaseEngine
{
public:
    enum class DebugRenderMode : int
    {
        Lit = 0,
        Albedo = 1,
        Normal = 2,
        Uv = 3,
        Depth = 4
    };

    enum class ProjectionMode : int
    {
        Orthographic = 0,
        Perspective = 1
    };

    explicit CameraEngine(Core* core);
    ~CameraEngine();

    void initialize();
    void shutdown();
    void updateFromInput(float deltaTimeSeconds);
    void updateUniformBuffer(uint32_t currentFrame);
    void setDebugRenderMode(DebugRenderMode mode);
    DebugRenderMode debugRenderMode() const;
    void setProjectionMode(ProjectionMode mode);
    void toggleProjectionMode();
    ProjectionMode projectionMode() const;

    VkDescriptorSetLayout descriptorSetLayoutHandle() const;
    VkDescriptorSet descriptorSetForFrame(uint32_t frameIndex) const;

private:
    float position_[3] = { 0.0f, 0.0f, 2.5f };
    float movementSpeed_ = 1.8f;
    float zoomSpeed_ = 1.2f;
    DebugRenderMode debugRenderMode_ = DebugRenderMode::Lit;
    ProjectionMode projectionMode_ = ProjectionMode::Orthographic;
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
