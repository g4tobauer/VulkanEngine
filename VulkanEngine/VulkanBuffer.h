#pragma once
#ifndef ENGINE_VULKAN_BUFFER
#define ENGINE_VULKAN_BUFFER

#include "CommonHeaders.h"

class Core;

class VulkanBuffer
{
public:
    VulkanBuffer() = default;

    void create(
        Core& core,
        VkDeviceSize size,
        VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties);
    void destroy(Core& core);

    void map(Core& core);
    void unmap(Core& core);
    void write(const void* sourceData, size_t byteCount, size_t offset = 0);

    VkBuffer handle() const;
    VkDeviceMemory memoryHandle() const;
    VkDeviceSize size() const;
    void* mappedData() const;

private:
    VkBuffer buffer_ = VK_NULL_HANDLE;
    VkDeviceMemory memory_ = VK_NULL_HANDLE;
    VkDeviceSize size_ = 0;
    void* mappedData_ = nullptr;
};

#endif // !ENGINE_VULKAN_BUFFER
