#pragma once
#ifndef ENGINE_GEOMETRY
#define ENGINE_GEOMETRY

#include "BaseEngine.h"
#include "Mesh.h"
#include "VulkanBuffer.h"

class GeometryEngine : BaseEngine
{
public:
    explicit GeometryEngine(Core* core);
    ~GeometryEngine();

    void createTriangleGeometry();
    void createMeshGeometry(const Mesh& mesh);
    void createSceneGeometry(const std::vector<Mesh>& meshes);
    void destroyGeometry();

    size_t meshCount() const;
    VkBuffer vertexBufferHandle(size_t meshIndex) const;
    VkBuffer indexBufferHandle(size_t meshIndex) const;
    uint32_t indexCount(size_t meshIndex) const;

private:
    struct GeometryResource
    {
        VulkanBuffer vertexBuffer;
        VulkanBuffer indexBuffer;
        uint32_t indexCount = 0;
    };

    std::vector<GeometryResource> geometryResources_;
    void createMesh(const Mesh& sourceMesh);
};

#endif // !ENGINE_GEOMETRY
