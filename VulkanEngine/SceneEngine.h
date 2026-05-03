#pragma once
#ifndef ENGINE_SCENE
#define ENGINE_SCENE

#include "BaseEngine.h"
#include "Mesh.h"

class SceneEngine : BaseEngine
{
public:
    explicit SceneEngine(Core* core);
    ~SceneEngine();

    void createDefaultScene();
    void clear();

    Mesh& primaryMesh();
    const Mesh& primaryMesh() const;
    std::vector<Mesh>& renderMeshes();
    const std::vector<Mesh>& renderMeshes() const;
    Mat4 primaryModelMatrix(float additionalRotationRadians = 0.0f) const;
    Mat4 modelMatrixForMesh(size_t meshIndex, float additionalRotationRadians = 0.0f) const;

private:
    std::vector<Mesh> renderMeshes_;
};

#endif // !ENGINE_SCENE
