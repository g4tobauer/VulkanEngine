#pragma once
#ifndef ENGINE_GEOMETRY
#define ENGINE_GEOMETRY

#include "BaseEngine.h"
#include "SceneObject.h"
#include "VulkanBuffer.h"

class SceneEngine;

class GeometryEngine : BaseEngine
{
public:
    explicit GeometryEngine(Core* core);
    ~GeometryEngine();

    void createTriangleGeometry();
    void createMeshGeometry(const Mesh& mesh);
    void createSceneGeometry(const SceneEngine& scene);
    void destroyGeometry();

    size_t meshCount() const;
    VkBuffer vertexBufferHandle(MeshAssetId meshAssetId) const;
    VkBuffer indexBufferHandle(MeshAssetId meshAssetId) const;
    uint32_t indexCount(MeshAssetId meshAssetId) const;

private:
    struct GeometryResource
    {
        MeshAssetId meshAssetId = 0;
        VulkanBuffer vertexBuffer;
        VulkanBuffer indexBuffer;
        uint32_t indexCount = 0;
    };

    std::vector<GeometryResource> geometryResources_;
    void createMesh(MeshAssetId meshAssetId, const Mesh& sourceMesh);
    const GeometryResource& geometryForAsset(MeshAssetId meshAssetId) const;
};

#endif // !ENGINE_GEOMETRY
