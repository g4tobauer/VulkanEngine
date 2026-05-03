#pragma once
#ifndef ENGINE_MESH
#define ENGINE_MESH

#include "CommonHeaders.h"

struct Mesh
{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    Transform transform;

    static Mesh createColoredTriangle()
    {
        Mesh mesh{};
        mesh.vertices = {
            Vertex{{ 0.0f, -0.5f }, { 1.0f, 0.0f, 0.0f }},
            Vertex{{ 0.5f,  0.5f }, { 0.0f, 1.0f, 0.0f }},
            Vertex{{-0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f }}
        };
        mesh.indices = { 0, 1, 2 };
        mesh.transform.scale[0] = 0.85f;
        mesh.transform.scale[1] = 0.85f;
        mesh.transform.scale[2] = 1.0f;
        return mesh;
    }

    static Mesh createOffsetTriangle(float offsetX, float offsetY, float scaleMultiplier)
    {
        Mesh mesh = createColoredTriangle();
        mesh.transform.position[0] = offsetX;
        mesh.transform.position[1] = offsetY;
        mesh.transform.scale[0] *= scaleMultiplier;
        mesh.transform.scale[1] *= scaleMultiplier;
        return mesh;
    }
};

#endif // !ENGINE_MESH
