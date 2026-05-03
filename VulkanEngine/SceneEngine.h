#pragma once
#ifndef ENGINE_SCENE
#define ENGINE_SCENE

#include "BaseEngine.h"
#include "SceneObject.h"

class SceneEngine : BaseEngine
{
public:
    explicit SceneEngine(Core* core);
    ~SceneEngine();

    void createDefaultScene();
    void clear();
    EntityId createObject(
        const std::string& name,
        MeshAssetId meshAssetId,
        MaterialAssetId materialAssetId,
        const Transform& transform = {});
    bool removeObject(EntityId id);
    SceneObject* findObject(EntityId id);
    const SceneObject* findObject(EntityId id) const;
    bool updateTransform(EntityId id, const Transform& transform);
    bool isGeometryDirty() const;
    bool isTransformDirty() const;
    void clearGeometryDirty();
    void clearTransformDirty();

    SceneObject& primaryObject();
    const SceneObject& primaryObject() const;
    std::vector<SceneObject>& renderObjects();
    const std::vector<SceneObject>& renderObjects() const;
    std::vector<MeshAssetId> referencedMeshAssetIds() const;
    Mat4 primaryModelMatrix(float additionalRotationRadians = 0.0f) const;
    Mat4 modelMatrixForObject(size_t objectIndex, float additionalRotationRadians = 0.0f) const;

private:
    EntityId nextEntityId_ = 1;
    bool geometryDirty_ = false;
    bool transformDirty_ = false;
    std::vector<SceneObject> renderObjects_;
};

#endif // !ENGINE_SCENE
