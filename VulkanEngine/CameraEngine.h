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
    void reset();

    VkDescriptorSetLayout descriptorSetLayoutHandle() const;
    VkDescriptorSet descriptorSetForFrame(uint32_t frameIndex) const;

private:
    float position_[3] = { 0.0f, 0.0f, 3.25f };
    float movementSpeed_ = 2.2f;
    float zoomSpeed_ = 1.6f;
    float yawRadians_ = 0.0f;
    float pitchRadians_ = 0.0f;
    float mouseSensitivity_ = 0.0035f;
    bool mouseLookActive_ = false;
    double lastCursorX_ = 0.0;
    double lastCursorY_ = 0.0;
    DebugRenderMode debugRenderMode_ = DebugRenderMode::Lit;
    ProjectionMode projectionMode_ = ProjectionMode::Perspective;
    VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
    std::vector<VulkanBuffer> uniformBuffers;
    std::vector<VkDescriptorSet> descriptorSets;

    void createDescriptorSetLayout();
    void createUniformBuffers();
    void createDescriptorPool();
    void createDescriptorSets();
    void perspectiveBasis(float& forwardX, float& forwardY, float& forwardZ, float& rightX, float& rightY, float& rightZ, float& upX, float& upY, float& upZ) const;
};

#endif // !ENGINE_CAMERA
