#include "AssetManager.h"
#include "Core.h"
#include "VulkanBuffer.h"

namespace
{
    constexpr VkFormat kTextureFormat = VK_FORMAT_R8G8B8A8_UNORM;
}

AssetManager::AssetManager(Core* core)
{
    pCore = core;
}

AssetManager::~AssetManager()
{
    pCore = NULL;
}

MeshAssetId AssetManager::createMeshAsset(const std::string& name, const Mesh& mesh)
{
    MeshAssetRecord asset{};
    asset.id = nextMeshAssetId_++;
    asset.name = name;
    asset.mesh = mesh;
    meshAssets_.push_back(asset);
    return asset.id;
}

const Mesh* AssetManager::findMeshAsset(MeshAssetId id) const
{
    for (const MeshAssetRecord& asset : meshAssets_)
    {
        if (asset.id == id)
        {
            return &asset.mesh;
        }
    }

    return nullptr;
}

void AssetManager::clearMeshAssets()
{
    nextMeshAssetId_ = 1;
    meshAssets_.clear();
}

MaterialAssetId AssetManager::createMaterialAsset(const std::string& name, const Material& material)
{
    MaterialAssetRecord asset{};
    asset.id = nextMaterialAssetId_++;
    asset.name = name;
    asset.material = material;
    materialAssets_.push_back(asset);
    return asset.id;
}

const Material* AssetManager::findMaterialAsset(MaterialAssetId id) const
{
    for (const MaterialAssetRecord& asset : materialAssets_)
    {
        if (asset.id == id)
        {
            return &asset.material;
        }
    }

    return nullptr;
}

void AssetManager::clearMaterialAssets()
{
    nextMaterialAssetId_ = 1;
    materialAssets_.clear();
}

TextureAssetId AssetManager::createTextureAsset(const std::string& name, const std::string& sourcePath)
{
    TextureAssetRecord asset{};
    asset.id = nextTextureAssetId_++;
    asset.name = name;
    asset.sourcePath = sourcePath;
    textureAssets_.push_back(asset);
    return asset.id;
}

std::string AssetManager::findTextureAssetSourcePath(TextureAssetId id) const
{
    for (const TextureAssetRecord& asset : textureAssets_)
    {
        if (asset.id == id)
        {
            return asset.sourcePath;
        }
    }

    return "";
}

void AssetManager::clearTextureAssets()
{
    nextTextureAssetId_ = 1;
    textureAssets_.clear();
}

void AssetManager::initializeGpuResources()
{
    shutdownGpuResources();
    createTextureDescriptorSetLayout();
    createTextureDescriptorPool();
    uploadTextureAssets();
    createMaterialDescriptorSets();
}

void AssetManager::shutdownGpuResources()
{
    VkDevice device = pCore->device().deviceHandle();

    for (TextureAssetRecord& texture : textureAssets_)
    {
        destroyTextureAsset(texture);
    }

    destroyTextureAsset(fallbackTexture_);
    fallbackTextureDescriptorSet_ = VK_NULL_HANDLE;

    for (MaterialAssetRecord& material : materialAssets_)
    {
        material.descriptorSet = VK_NULL_HANDLE;
    }

    if (textureDescriptorPool_ != VK_NULL_HANDLE)
    {
        vkDestroyDescriptorPool(device, textureDescriptorPool_, nullptr);
        textureDescriptorPool_ = VK_NULL_HANDLE;
    }

    if (textureDescriptorSetLayout_ != VK_NULL_HANDLE)
    {
        vkDestroyDescriptorSetLayout(device, textureDescriptorSetLayout_, nullptr);
        textureDescriptorSetLayout_ = VK_NULL_HANDLE;
    }
}

VkDescriptorSetLayout AssetManager::textureDescriptorSetLayoutHandle() const
{
    return textureDescriptorSetLayout_;
}

VkDescriptorSet AssetManager::materialDescriptorSet(MaterialAssetId id) const
{
    for (const MaterialAssetRecord& material : materialAssets_)
    {
        if (material.id == id)
        {
            return material.descriptorSet;
        }
    }

    return fallbackTextureDescriptorSet_;
}

void AssetManager::createTextureDescriptorSetLayout()
{
    VkDescriptorSetLayoutBinding samplerLayoutBinding{};
    samplerLayoutBinding.binding = 0;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &samplerLayoutBinding;

    if (vkCreateDescriptorSetLayout(pCore->device().deviceHandle(), &layoutInfo, nullptr, &textureDescriptorSetLayout_) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create texture descriptor set layout!");
    }
}

void AssetManager::createTextureDescriptorPool()
{
    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSize.descriptorCount = static_cast<uint32_t>(materialAssets_.size() + 1);

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = static_cast<uint32_t>(materialAssets_.size() + 1);

    if (vkCreateDescriptorPool(pCore->device().deviceHandle(), &poolInfo, nullptr, &textureDescriptorPool_) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create texture descriptor pool!");
    }
}

void AssetManager::uploadTextureAssets()
{
    createPlaceholderTexture(fallbackTexture_, false);

    for (TextureAssetRecord& texture : textureAssets_)
    {
        const bool checkerPattern = texture.sourcePath.find("checker") != std::string::npos;
        createPlaceholderTexture(texture, checkerPattern);
    }
}

void AssetManager::createMaterialDescriptorSets()
{
    fallbackTextureDescriptorSet_ = allocateTextureDescriptorSet();

    VkDescriptorImageInfo fallbackImageInfo{};
    fallbackImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    fallbackImageInfo.imageView = fallbackTexture_.imageView;
    fallbackImageInfo.sampler = fallbackTexture_.sampler;

    VkWriteDescriptorSet fallbackWrite{};
    fallbackWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    fallbackWrite.dstSet = fallbackTextureDescriptorSet_;
    fallbackWrite.dstBinding = 0;
    fallbackWrite.descriptorCount = 1;
    fallbackWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    fallbackWrite.pImageInfo = &fallbackImageInfo;
    vkUpdateDescriptorSets(pCore->device().deviceHandle(), 1, &fallbackWrite, 0, nullptr);

    for (MaterialAssetRecord& material : materialAssets_)
    {
        const TextureAssetRecord* selectedTexture = &fallbackTexture_;

        if (material.material.baseColorTextureAssetId != 0)
        {
            for (const TextureAssetRecord& texture : textureAssets_)
            {
                if (texture.id == material.material.baseColorTextureAssetId)
                {
                    selectedTexture = &texture;
                    break;
                }
            }
        }

        material.descriptorSet = allocateTextureDescriptorSet();

        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = selectedTexture->imageView;
        imageInfo.sampler = selectedTexture->sampler;

        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = material.descriptorSet;
        descriptorWrite.dstBinding = 0;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrite.pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(pCore->device().deviceHandle(), 1, &descriptorWrite, 0, nullptr);
    }
}

void AssetManager::createPlaceholderTexture(TextureAssetRecord& textureRecord, bool checkerPattern)
{
    const uint32_t width = checkerPattern ? 2 : 1;
    const uint32_t height = checkerPattern ? 2 : 1;
    std::vector<uint8_t> pixels(width * height * 4, 255);

    if (checkerPattern)
    {
        const std::array<uint8_t, 16> checkerPixels = {
            255, 255, 255, 255,
             32,  32,  32, 255,
             32,  32,  32, 255,
            255, 255, 255, 255
        };
        pixels.assign(checkerPixels.begin(), checkerPixels.end());
    }

    VulkanBuffer stagingBuffer;
    stagingBuffer.create(
        *pCore,
        pixels.size(),
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    stagingBuffer.map(*pCore);
    stagingBuffer.write(pixels.data(), pixels.size());

    createImage(
        width,
        height,
        kTextureFormat,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        textureRecord.image,
        textureRecord.memory);

    transitionImageLayout(textureRecord.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copyBufferToImage(stagingBuffer.handle(), textureRecord.image, width, height);
    transitionImageLayout(textureRecord.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    textureRecord.imageView = createImageView(textureRecord.image, kTextureFormat);
    textureRecord.sampler = createSampler();

    stagingBuffer.destroy(*pCore);
}

void AssetManager::destroyTextureAsset(TextureAssetRecord& textureRecord)
{
    VkDevice device = pCore->device().deviceHandle();

    if (textureRecord.sampler != VK_NULL_HANDLE)
    {
        vkDestroySampler(device, textureRecord.sampler, nullptr);
        textureRecord.sampler = VK_NULL_HANDLE;
    }

    if (textureRecord.imageView != VK_NULL_HANDLE)
    {
        vkDestroyImageView(device, textureRecord.imageView, nullptr);
        textureRecord.imageView = VK_NULL_HANDLE;
    }

    if (textureRecord.image != VK_NULL_HANDLE)
    {
        vkDestroyImage(device, textureRecord.image, nullptr);
        textureRecord.image = VK_NULL_HANDLE;
    }

    if (textureRecord.memory != VK_NULL_HANDLE)
    {
        vkFreeMemory(device, textureRecord.memory, nullptr);
        textureRecord.memory = VK_NULL_HANDLE;
    }
}

void AssetManager::createImage(
    uint32_t width,
    uint32_t height,
    VkFormat format,
    VkImageTiling tiling,
    VkImageUsageFlags usage,
    VkMemoryPropertyFlags properties,
    VkImage& image,
    VkDeviceMemory& imageMemory)
{
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

    if (vkCreateImage(pCore->device().deviceHandle(), &imageInfo, nullptr, &image) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create image!");
    }

    VkMemoryRequirements memoryRequirements{};
    vkGetImageMemoryRequirements(pCore->device().deviceHandle(), image, &memoryRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memoryRequirements.size;
    allocInfo.memoryTypeIndex = pCore->device().findMemoryType(memoryRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(pCore->device().deviceHandle(), &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate image memory!");
    }

    vkBindImageMemory(pCore->device().deviceHandle(), image, imageMemory, 0);
}

VkImageView AssetManager::createImageView(VkImage image, VkFormat format) const
{
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView = VK_NULL_HANDLE;
    if (vkCreateImageView(pCore->device().deviceHandle(), &viewInfo, nullptr, &imageView) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create texture image view!");
    }

    return imageView;
}

VkSampler AssetManager::createSampler() const
{
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_FALSE;
    samplerInfo.maxAnisotropy = 1.0f;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;
    samplerInfo.mipLodBias = 0.0f;

    VkSampler sampler = VK_NULL_HANDLE;
    if (vkCreateSampler(pCore->device().deviceHandle(), &samplerInfo, nullptr, &sampler) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create texture sampler!");
    }

    return sampler;
}

void AssetManager::transitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout)
{
    VkCommandBuffer commandBuffer = pCore->commandPool().beginSingleTimeCommands();

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    VkPipelineStageFlags destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else
    {
        throw std::runtime_error("unsupported image layout transition!");
    }

    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage,
        destinationStage,
        0,
        0,
        nullptr,
        0,
        nullptr,
        1,
        &barrier);

    pCore->commandPool().endSingleTimeCommands(commandBuffer);
}

void AssetManager::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{
    VkCommandBuffer commandBuffer = pCore->commandPool().beginSingleTimeCommands();

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = { width, height, 1 };

    vkCmdCopyBufferToImage(
        commandBuffer,
        buffer,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region);

    pCore->commandPool().endSingleTimeCommands(commandBuffer);
}

VkDescriptorSet AssetManager::allocateTextureDescriptorSet()
{
    VkDescriptorSetLayout layout = textureDescriptorSetLayout_;
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = textureDescriptorPool_;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &layout;

    VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
    if (vkAllocateDescriptorSets(pCore->device().deviceHandle(), &allocInfo, &descriptorSet) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate texture descriptor set!");
    }

    return descriptorSet;
}
