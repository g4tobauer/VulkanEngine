#pragma once
#ifndef ENGINE_SCENE_OBJECT
#define ENGINE_SCENE_OBJECT

#include "EngineIds.h"
#include "Mesh.h"
struct MeshRendererComponent
{
    MeshAssetId meshAssetId = 0;
    MaterialAssetId materialAssetId = 0;
};

struct SceneObject
{
    EntityId id = 0;
    std::string name;
    Transform transform;
    MeshRendererComponent meshRenderer;

    Mat4 modelMatrix(float additionalRotationRadians = 0.0f) const
    {
        return multiply(
            transform.toMatrix(),
            Mat4::rotationZ(additionalRotationRadians));
    }
};

#endif // !ENGINE_SCENE_OBJECT
