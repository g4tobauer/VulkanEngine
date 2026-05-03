#include "SceneEngine.h"
#include "Core.h"

SceneEngine::SceneEngine(Core* core)
{
    pCore = core;
}

SceneEngine::~SceneEngine()
{
    pCore = NULL;
}

void SceneEngine::createDefaultScene()
{
    clear();

    pCore->assets().clearMeshAssets();
    pCore->assets().clearMaterialAssets();
    pCore->assets().clearTextureAssets();
    const MeshAssetId cubeAssetId = pCore->assets().createMeshAsset("CubeMesh", Mesh::createColoredCube());
    const TextureAssetId checkerTextureId = pCore->assets().createTextureAsset("CheckerPlaceholder", "assets/textures/checker_placeholder.png");
    Material leftMaterial{};
    leftMaterial.baseColor[0] = 1.0f;
    leftMaterial.baseColor[1] = 0.85f;
    leftMaterial.baseColor[2] = 0.75f;
    leftMaterial.baseColorTextureAssetId = checkerTextureId;

    Material rightMaterial{};
    rightMaterial.baseColor[0] = 0.72f;
    rightMaterial.baseColor[1] = 0.88f;
    rightMaterial.baseColor[2] = 1.0f;
    rightMaterial.baseColorTextureAssetId = checkerTextureId;

    const MaterialAssetId leftMaterialId = pCore->assets().createMaterialAsset("LeftMaterial", leftMaterial);
    const MaterialAssetId rightMaterialId = pCore->assets().createMaterialAsset("RightMaterial", rightMaterial);

    Transform leftTransform{};
    leftTransform.position[0] = -0.5f;
    leftTransform.position[1] = 0.0f;
    leftTransform.position[2] = 0.15f;
    leftTransform.scale[0] = 0.55f;
    leftTransform.scale[1] = 0.55f;
    leftTransform.scale[2] = 0.55f;
    leftTransform.rotationRadians = -0.18f;

    Transform rightTransform{};
    rightTransform.position[0] = 0.5f;
    rightTransform.position[1] = 0.0f;
    rightTransform.position[2] = -0.15f;
    rightTransform.scale[0] = 0.55f;
    rightTransform.scale[1] = 0.55f;
    rightTransform.scale[2] = 0.55f;
    rightTransform.rotationRadians = 0.3f;

    createObject("LeftCube", cubeAssetId, leftMaterialId, leftTransform);
    createObject("RightCube", cubeAssetId, rightMaterialId, rightTransform);
}

void SceneEngine::clear()
{
    nextEntityId_ = 1;
    renderObjects_.clear();
    geometryDirty_ = true;
    transformDirty_ = true;
}

EntityId SceneEngine::createObject(
    const std::string& name,
    MeshAssetId meshAssetId,
    MaterialAssetId materialAssetId,
    const Transform& transform)
{
    if (pCore->assets().findMeshAsset(meshAssetId) == nullptr)
    {
        throw std::runtime_error("cannot create scene object with invalid mesh asset id!");
    }
    if (pCore->assets().findMaterialAsset(materialAssetId) == nullptr)
    {
        throw std::runtime_error("cannot create scene object with invalid material asset id!");
    }

    SceneObject object{};
    object.id = nextEntityId_++;
    object.name = name;
    object.transform = transform;
    object.meshRenderer.meshAssetId = meshAssetId;
    object.meshRenderer.materialAssetId = materialAssetId;

    renderObjects_.push_back(object);
    transformDirty_ = true;
    return object.id;
}

bool SceneEngine::removeObject(EntityId id)
{
    const auto newEnd = std::remove_if(
        renderObjects_.begin(),
        renderObjects_.end(),
        [id](const SceneObject& object)
        {
            return object.id == id;
        });

    if (newEnd == renderObjects_.end())
    {
        return false;
    }

    renderObjects_.erase(newEnd, renderObjects_.end());
    geometryDirty_ = true;
    transformDirty_ = true;
    return true;
}

bool SceneEngine::updateTransform(EntityId id, const Transform& transform)
{
    SceneObject* object = findObject(id);
    if (object == nullptr)
    {
        return false;
    }

    object->transform = transform;
    transformDirty_ = true;
    return true;
}

SceneObject* SceneEngine::findObject(EntityId id)
{
    for (SceneObject& object : renderObjects_)
    {
        if (object.id == id)
        {
            return &object;
        }
    }

    return nullptr;
}

const SceneObject* SceneEngine::findObject(EntityId id) const
{
    for (const SceneObject& object : renderObjects_)
    {
        if (object.id == id)
        {
            return &object;
        }
    }

    return nullptr;
}

SceneObject& SceneEngine::primaryObject()
{
    if (renderObjects_.empty())
    {
        throw std::runtime_error("scene has no render objects!");
    }

    return renderObjects_.front();
}

const SceneObject& SceneEngine::primaryObject() const
{
    if (renderObjects_.empty())
    {
        throw std::runtime_error("scene has no render objects!");
    }

    return renderObjects_.front();
}

std::vector<SceneObject>& SceneEngine::renderObjects()
{
    return renderObjects_;
}

const std::vector<SceneObject>& SceneEngine::renderObjects() const
{
    return renderObjects_;
}

std::vector<MeshAssetId> SceneEngine::referencedMeshAssetIds() const
{
    std::vector<MeshAssetId> assetIds;
    std::set<MeshAssetId> uniqueAssetIds;

    for (const SceneObject& object : renderObjects_)
    {
        if (uniqueAssetIds.insert(object.meshRenderer.meshAssetId).second)
        {
            assetIds.push_back(object.meshRenderer.meshAssetId);
        }
    }

    return assetIds;
}

Mat4 SceneEngine::primaryModelMatrix(float additionalRotationRadians) const
{
    return primaryObject().modelMatrix(additionalRotationRadians);
}

Mat4 SceneEngine::modelMatrixForObject(size_t objectIndex, float additionalRotationRadians) const
{
    if (objectIndex >= renderObjects_.size())
    {
        throw std::runtime_error("scene object index out of range!");
    }

    return renderObjects_[objectIndex].modelMatrix(additionalRotationRadians);
}

bool SceneEngine::isGeometryDirty() const
{
    return geometryDirty_;
}

bool SceneEngine::isTransformDirty() const
{
    return transformDirty_;
}

void SceneEngine::clearGeometryDirty()
{
    geometryDirty_ = false;
}

void SceneEngine::clearTransformDirty()
{
    transformDirty_ = false;
}
