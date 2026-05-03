#include "VulkanBuffer.h"
#include "Core.h"

void VulkanBuffer::create(
    Core& core,
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties)
{
    size_ = size;

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(core.device().deviceHandle(), &bufferInfo, nullptr, &buffer_) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create Vulkan buffer!");
    }

    VkMemoryRequirements memoryRequirements{};
    vkGetBufferMemoryRequirements(core.device().deviceHandle(), buffer_, &memoryRequirements);

    VkMemoryAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.allocationSize = memoryRequirements.size;
    allocateInfo.memoryTypeIndex = core.device().findMemoryType(memoryRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(core.device().deviceHandle(), &allocateInfo, nullptr, &memory_) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate Vulkan buffer memory!");
    }

    vkBindBufferMemory(core.device().deviceHandle(), buffer_, memory_, 0);
}

void VulkanBuffer::destroy(Core& core)
{
    unmap(core);

    if (buffer_ != VK_NULL_HANDLE)
    {
        vkDestroyBuffer(core.device().deviceHandle(), buffer_, nullptr);
        buffer_ = VK_NULL_HANDLE;
    }

    if (memory_ != VK_NULL_HANDLE)
    {
        vkFreeMemory(core.device().deviceHandle(), memory_, nullptr);
        memory_ = VK_NULL_HANDLE;
    }

    size_ = 0;
}

void VulkanBuffer::map(Core& core)
{
    if (mappedData_ == nullptr)
    {
        vkMapMemory(core.device().deviceHandle(), memory_, 0, size_, 0, &mappedData_);
    }
}

void VulkanBuffer::unmap(Core& core)
{
    if (mappedData_ != nullptr)
    {
        vkUnmapMemory(core.device().deviceHandle(), memory_);
        mappedData_ = nullptr;
    }
}

void VulkanBuffer::write(const void* sourceData, size_t byteCount, size_t offset)
{
    if (mappedData_ == nullptr)
    {
        throw std::runtime_error("attempted to write to an unmapped Vulkan buffer!");
    }

    memcpy(static_cast<char*>(mappedData_) + offset, sourceData, byteCount);
}

VkBuffer VulkanBuffer::handle() const
{
    return buffer_;
}

VkDeviceMemory VulkanBuffer::memoryHandle() const
{
    return memory_;
}

VkDeviceSize VulkanBuffer::size() const
{
    return size_;
}

void* VulkanBuffer::mappedData() const
{
    return mappedData_;
}
