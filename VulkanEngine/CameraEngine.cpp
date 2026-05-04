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

void CameraEngine::updateFromInput(float deltaTimeSeconds)
{
    const float movementStep = movementSpeed_ * deltaTimeSeconds;
    const float zoomStep = zoomSpeed_ * deltaTimeSeconds;

    if (projectionMode_ == ProjectionMode::Orthographic)
    {
        if (pCore->window().isKeyPressed(GLFW_KEY_A))
        {
            position_[0] -= movementStep;
        }
        if (pCore->window().isKeyPressed(GLFW_KEY_D))
        {
            position_[0] += movementStep;
        }
        if (pCore->window().isKeyPressed(GLFW_KEY_W))
        {
            position_[1] += movementStep;
        }
        if (pCore->window().isKeyPressed(GLFW_KEY_S))
        {
            position_[1] -= movementStep;
        }
        if (pCore->window().isKeyPressed(GLFW_KEY_Q))
        {
            position_[2] = std::min(position_[2] + zoomStep, 6.0f);
        }
        if (pCore->window().isKeyPressed(GLFW_KEY_E))
        {
            position_[2] = std::max(position_[2] - zoomStep, 0.8f);
        }
        return;
    }

    double cursorX = 0.0;
    double cursorY = 0.0;
    pCore->window().cursorPosition(cursorX, cursorY);
    const bool mouseLookPressed = pCore->window().isMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT);

    if (mouseLookPressed)
    {
        if (!mouseLookActive_)
        {
            mouseLookActive_ = true;
            lastCursorX_ = cursorX;
            lastCursorY_ = cursorY;
        }
        else
        {
            const double deltaX = cursorX - lastCursorX_;
            const double deltaY = cursorY - lastCursorY_;
            yawRadians_ += static_cast<float>(deltaX) * mouseSensitivity_;
            pitchRadians_ -= static_cast<float>(deltaY) * mouseSensitivity_;
            pitchRadians_ = std::clamp(pitchRadians_, -1.4f, 1.4f);
            lastCursorX_ = cursorX;
            lastCursorY_ = cursorY;
        }
    }
    else
    {
        mouseLookActive_ = false;
        lastCursorX_ = cursorX;
        lastCursorY_ = cursorY;
    }

    float forwardX = 0.0f;
    float forwardY = 0.0f;
    float forwardZ = 0.0f;
    float rightX = 0.0f;
    float rightY = 0.0f;
    float rightZ = 0.0f;
    float upX = 0.0f;
    float upY = 0.0f;
    float upZ = 0.0f;
    perspectiveBasis(forwardX, forwardY, forwardZ, rightX, rightY, rightZ, upX, upY, upZ);

    if (pCore->window().isKeyPressed(GLFW_KEY_W))
    {
        position_[0] += forwardX * zoomStep;
        position_[1] += forwardY * zoomStep;
        position_[2] += forwardZ * zoomStep;
    }
    if (pCore->window().isKeyPressed(GLFW_KEY_S))
    {
        position_[0] -= forwardX * zoomStep;
        position_[1] -= forwardY * zoomStep;
        position_[2] -= forwardZ * zoomStep;
    }
    if (pCore->window().isKeyPressed(GLFW_KEY_A))
    {
        position_[0] -= rightX * movementStep;
        position_[1] -= rightY * movementStep;
        position_[2] -= rightZ * movementStep;
    }
    if (pCore->window().isKeyPressed(GLFW_KEY_D))
    {
        position_[0] += rightX * movementStep;
        position_[1] += rightY * movementStep;
        position_[2] += rightZ * movementStep;
    }
    if (pCore->window().isKeyPressed(GLFW_KEY_Q))
    {
        position_[1] -= movementStep;
    }
    if (pCore->window().isKeyPressed(GLFW_KEY_E))
    {
        position_[1] += movementStep;
    }
}

void CameraEngine::updateUniformBuffer(uint32_t currentFrame)
{
    UniformBufferObject ubo{};
    const VkExtent2D extent = pCore->swapChain().swapChainExtentValue();
    const float aspectRatio = extent.height == 0
        ? 1.0f
        : static_cast<float>(extent.width) / static_cast<float>(extent.height);

    constexpr float kFovRadians = 3.14159265f / 3.0f;
    constexpr float kNearPlane = 0.1f;
    constexpr float kFarPlane = 10.0f;

    float forwardX = 0.0f;
    float forwardY = 0.0f;
    float forwardZ = 0.0f;
    float rightX = 0.0f;
    float rightY = 0.0f;
    float rightZ = 0.0f;
    float upX = 0.0f;
    float upY = 0.0f;
    float upZ = 0.0f;
    perspectiveBasis(forwardX, forwardY, forwardZ, rightX, rightY, rightZ, upX, upY, upZ);

    Mat4 view = Mat4::translation(-position_[0], -position_[1], -position_[2]);
    Mat4 projection = Mat4::perspective(kFovRadians, aspectRatio, kNearPlane, kFarPlane);
    Mat4 viewProjection = multiply(view, projection);

    ubo.view = transpose(view);
    ubo.projection = transpose(projection);
    ubo.viewProjection = transpose(viewProjection);
    ubo.cameraPosition[0] = position_[0];
    ubo.cameraPosition[1] = position_[1];
    ubo.cameraPosition[2] = position_[2];
    ubo.cameraPosition[3] = 1.0f;
    ubo.cameraForward[0] = forwardX;
    ubo.cameraForward[1] = forwardY;
    ubo.cameraForward[2] = forwardZ;
    ubo.cameraForward[3] = 0.0f;
    ubo.cameraRight[0] = rightX;
    ubo.cameraRight[1] = rightY;
    ubo.cameraRight[2] = rightZ;
    ubo.cameraRight[3] = 0.0f;
    ubo.cameraUp[0] = upX;
    ubo.cameraUp[1] = upY;
    ubo.cameraUp[2] = upZ;
    ubo.cameraUp[3] = 0.0f;
    ubo.projectionParams[0] = aspectRatio;
    ubo.projectionParams[1] = std::tan(kFovRadians * 0.5f);
    ubo.projectionParams[2] = kNearPlane;
    ubo.projectionParams[3] = kFarPlane;
    ubo.cameraOptions[0] = static_cast<float>(projectionMode_);
    ubo.cameraOptions[1] = position_[2];
    ubo.lightDirection[0] = -0.35f;
    ubo.lightDirection[1] = -0.55f;
    ubo.lightDirection[2] = -0.75f;
    ubo.lightDirection[3] = 0.0f;
    ubo.lightColor[0] = 1.0f;
    ubo.lightColor[1] = 0.97f;
    ubo.lightColor[2] = 0.93f;
    ubo.lightColor[3] = 1.0f;
    ubo.ambientColor[0] = 0.24f;
    ubo.ambientColor[1] = 0.25f;
    ubo.ambientColor[2] = 0.28f;
    ubo.ambientColor[3] = 1.0f;
    ubo.debugOptions[0] = static_cast<float>(debugRenderMode_);

    uniformBuffers[currentFrame].write(&ubo, sizeof(ubo));
}

void CameraEngine::setDebugRenderMode(DebugRenderMode mode)
{
    debugRenderMode_ = mode;
}

CameraEngine::DebugRenderMode CameraEngine::debugRenderMode() const
{
    return debugRenderMode_;
}

void CameraEngine::setProjectionMode(ProjectionMode mode)
{
    projectionMode_ = mode;
}

void CameraEngine::toggleProjectionMode()
{
    projectionMode_ = projectionMode_ == ProjectionMode::Orthographic
        ? ProjectionMode::Perspective
        : ProjectionMode::Orthographic;
}

CameraEngine::ProjectionMode CameraEngine::projectionMode() const
{
    return projectionMode_;
}

void CameraEngine::reset()
{
    position_[0] = 0.0f;
    position_[1] = 0.0f;
    position_[2] = projectionMode_ == ProjectionMode::Perspective ? 3.25f : 2.2f;
    yawRadians_ = 0.0f;
    pitchRadians_ = 0.0f;
    mouseLookActive_ = false;
}

void CameraEngine::perspectiveBasis(
    float& forwardX,
    float& forwardY,
    float& forwardZ,
    float& rightX,
    float& rightY,
    float& rightZ,
    float& upX,
    float& upY,
    float& upZ) const
{
    forwardX = std::cos(pitchRadians_) * std::sin(yawRadians_);
    forwardY = std::sin(pitchRadians_);
    forwardZ = -std::cos(pitchRadians_) * std::cos(yawRadians_);

    const float rightLength = std::sqrt((forwardZ * forwardZ) + (forwardX * forwardX));
    rightX = rightLength > 0.0f ? -forwardZ / rightLength : 1.0f;
    rightY = 0.0f;
    rightZ = rightLength > 0.0f ? forwardX / rightLength : 0.0f;

    upX = (rightY * forwardZ) - (rightZ * forwardY);
    upY = (rightZ * forwardX) - (rightX * forwardZ);
    upZ = (rightX * forwardY) - (rightY * forwardX);
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
