#pragma once
#ifndef ENGINE_ASSET_MANAGER
#define ENGINE_ASSET_MANAGER

#include "BaseEngine.h"
#include "EngineIds.h"
#include "Mesh.h"

class AssetManager : BaseEngine
{
public:
    explicit AssetManager(Core* core);
    ~AssetManager();

    MeshAssetId createMeshAsset(const std::string& name, const Mesh& mesh);
    const Mesh* findMeshAsset(MeshAssetId id) const;
    void clearMeshAssets();
    MaterialAssetId createMaterialAsset(const std::string& name, const Material& material);
    const Material* findMaterialAsset(MaterialAssetId id) const;
    void clearMaterialAssets();
    TextureAssetId createTextureAsset(const std::string& name, const std::string& sourcePath);
    std::string findTextureAssetSourcePath(TextureAssetId id) const;
    void clearTextureAssets();
    void initializeGpuResources();
    void shutdownGpuResources();
    VkDescriptorSetLayout textureDescriptorSetLayoutHandle() const;
    VkDescriptorSet materialDescriptorSet(MaterialAssetId id) const;

private:
    struct MeshAssetRecord
    {
        MeshAssetId id = 0;
        std::string name;
        Mesh mesh;
    };

    MeshAssetId nextMeshAssetId_ = 1;
    MaterialAssetId nextMaterialAssetId_ = 1;
    std::vector<MeshAssetRecord> meshAssets_;

    struct MaterialAssetRecord
    {
        MaterialAssetId id = 0;
        std::string name;
        Material material;
        VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
    };

    std::vector<MaterialAssetRecord> materialAssets_;
    TextureAssetId nextTextureAssetId_ = 1;

    struct TextureAssetRecord
    {
        TextureAssetId id = 0;
        std::string name;
        std::string sourcePath;
        VkImage image = VK_NULL_HANDLE;
        VkDeviceMemory memory = VK_NULL_HANDLE;
        VkImageView imageView = VK_NULL_HANDLE;
        VkSampler sampler = VK_NULL_HANDLE;
    };

    std::vector<TextureAssetRecord> textureAssets_;
    VkDescriptorSetLayout textureDescriptorSetLayout_ = VK_NULL_HANDLE;
    VkDescriptorPool textureDescriptorPool_ = VK_NULL_HANDLE;
    VkDescriptorSet fallbackTextureDescriptorSet_ = VK_NULL_HANDLE;
    TextureAssetRecord fallbackTexture_;

    void createTextureDescriptorSetLayout();
    void createTextureDescriptorPool();
    void uploadTextureAssets();
    void createMaterialDescriptorSets();
    void createPlaceholderTexture(TextureAssetRecord& textureRecord, bool checkerPattern);
    void destroyTextureAsset(TextureAssetRecord& textureRecord);
    void createImage(
        uint32_t width,
        uint32_t height,
        VkFormat format,
        VkImageTiling tiling,
        VkImageUsageFlags usage,
        VkMemoryPropertyFlags properties,
        VkImage& image,
        VkDeviceMemory& imageMemory);
    VkImageView createImageView(VkImage image, VkFormat format) const;
    VkSampler createSampler() const;
    void transitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout);
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
    VkDescriptorSet allocateTextureDescriptorSet();
};

#endif // !ENGINE_ASSET_MANAGER
