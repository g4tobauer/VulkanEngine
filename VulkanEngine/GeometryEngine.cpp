#include "GeometryEngine.h"
#include "Core.h"
#include "SceneEngine.h"

GeometryEngine::GeometryEngine(Core* core)
{
    pCore = core;
}

GeometryEngine::~GeometryEngine()
{
    pCore = NULL;
}

void GeometryEngine::createTriangleGeometry()
{
    destroyGeometry();
    createMesh(1, Mesh::createColoredTriangle());
}

void GeometryEngine::createMeshGeometry(const Mesh& mesh)
{
    destroyGeometry();
    createMesh(1, mesh);
}

void GeometryEngine::createSceneGeometry(const SceneEngine& scene)
{
    destroyGeometry();

    for (MeshAssetId assetId : scene.referencedMeshAssetIds())
    {
        const Mesh* mesh = pCore->assets().findMeshAsset(assetId);
        if (mesh == nullptr)
        {
            throw std::runtime_error("scene referenced an invalid mesh asset!");
        }

        createMesh(assetId, *mesh);
    }
}

void GeometryEngine::destroyGeometry()
{
    for (GeometryResource& resource : geometryResources_)
    {
        resource.indexBuffer.destroy(*pCore);
        resource.vertexBuffer.destroy(*pCore);
    }

    geometryResources_.clear();
}

size_t GeometryEngine::meshCount() const
{
    return geometryResources_.size();
}

VkBuffer GeometryEngine::vertexBufferHandle(MeshAssetId meshAssetId) const
{
    return geometryForAsset(meshAssetId).vertexBuffer.handle();
}

VkBuffer GeometryEngine::indexBufferHandle(MeshAssetId meshAssetId) const
{
    return geometryForAsset(meshAssetId).indexBuffer.handle();
}

uint32_t GeometryEngine::indexCount(MeshAssetId meshAssetId) const
{
    return geometryForAsset(meshAssetId).indexCount;
}

void GeometryEngine::createMesh(MeshAssetId meshAssetId, const Mesh& sourceMesh)
{
    GeometryResource resource{};
    resource.meshAssetId = meshAssetId;
    resource.indexCount = static_cast<uint32_t>(sourceMesh.indices.size());

    const VkDeviceSize vertexBufferSize = sizeof(Vertex) * sourceMesh.vertices.size();
    resource.vertexBuffer.create(
        *pCore,
        vertexBufferSize,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    resource.vertexBuffer.map(*pCore);
    resource.vertexBuffer.write(sourceMesh.vertices.data(), static_cast<size_t>(vertexBufferSize));
    resource.vertexBuffer.unmap(*pCore);

    const VkDeviceSize indexBufferSize = sizeof(uint32_t) * sourceMesh.indices.size();
    resource.indexBuffer.create(
        *pCore,
        indexBufferSize,
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    resource.indexBuffer.map(*pCore);
    resource.indexBuffer.write(sourceMesh.indices.data(), static_cast<size_t>(indexBufferSize));
    resource.indexBuffer.unmap(*pCore);

    geometryResources_.push_back(std::move(resource));
}

const GeometryEngine::GeometryResource& GeometryEngine::geometryForAsset(MeshAssetId meshAssetId) const
{
    for (const GeometryResource& resource : geometryResources_)
    {
        if (resource.meshAssetId == meshAssetId)
        {
            return resource;
        }
    }

    throw std::runtime_error("geometry asset was not uploaded to GPU!");
}
