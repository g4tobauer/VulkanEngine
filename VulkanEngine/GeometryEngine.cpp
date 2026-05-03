#include "GeometryEngine.h"
#include "Core.h"

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
    createMesh(Mesh::createColoredTriangle());
}

void GeometryEngine::createMeshGeometry(const Mesh& mesh)
{
    destroyGeometry();
    createMesh(mesh);
}

void GeometryEngine::createSceneGeometry(const std::vector<Mesh>& meshes)
{
    destroyGeometry();

    for (const Mesh& mesh : meshes)
    {
        createMesh(mesh);
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

VkBuffer GeometryEngine::vertexBufferHandle(size_t meshIndex) const
{
    return geometryResources_.at(meshIndex).vertexBuffer.handle();
}

VkBuffer GeometryEngine::indexBufferHandle(size_t meshIndex) const
{
    return geometryResources_.at(meshIndex).indexBuffer.handle();
}

uint32_t GeometryEngine::indexCount(size_t meshIndex) const
{
    return geometryResources_.at(meshIndex).indexCount;
}

void GeometryEngine::createMesh(const Mesh& sourceMesh)
{
    GeometryResource resource{};
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
