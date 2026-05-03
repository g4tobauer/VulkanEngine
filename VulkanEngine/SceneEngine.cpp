#include "SceneEngine.h"

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
    renderMeshes_.clear();

    renderMeshes_.push_back(Mesh::createOffsetTriangle(-0.35f, 0.0f, 0.75f));
    renderMeshes_.push_back(Mesh::createOffsetTriangle(0.35f, 0.0f, 0.75f));
}

void SceneEngine::clear()
{
    renderMeshes_.clear();
}

Mesh& SceneEngine::primaryMesh()
{
    if (renderMeshes_.empty())
    {
        throw std::runtime_error("scene has no render meshes!");
    }

    return renderMeshes_.front();
}

const Mesh& SceneEngine::primaryMesh() const
{
    if (renderMeshes_.empty())
    {
        throw std::runtime_error("scene has no render meshes!");
    }

    return renderMeshes_.front();
}

std::vector<Mesh>& SceneEngine::renderMeshes()
{
    return renderMeshes_;
}

const std::vector<Mesh>& SceneEngine::renderMeshes() const
{
    return renderMeshes_;
}

Mat4 SceneEngine::primaryModelMatrix(float additionalRotationRadians) const
{
    const Mesh& mesh = primaryMesh();
    return multiply(mesh.transform.toMatrix(), Mat4::rotationZ(additionalRotationRadians));
}

Mat4 SceneEngine::modelMatrixForMesh(size_t meshIndex, float additionalRotationRadians) const
{
    if (meshIndex >= renderMeshes_.size())
    {
        throw std::runtime_error("scene mesh index out of range!");
    }

    return multiply(renderMeshes_[meshIndex].transform.toMatrix(), Mat4::rotationZ(additionalRotationRadians));
}
