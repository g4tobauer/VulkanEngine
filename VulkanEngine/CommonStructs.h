#pragma once
#ifndef ENGINE_COMMON_STRUCTS
#define ENGINE_COMMON_STRUCTS

#ifndef ENGINE_COMMON_HEADERS
#include "CommonHeaders.h"
#endif // !#ifndef ENGINE_COMMON_HEADERS


struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};


struct QueueFamilyIndices
{
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;
	float queuePriority = 1.0f;
	//int graphicsFamily = -1;
	//int presentFamily = -1;

	bool isComplete()
	{
		return graphicsFamily.has_value() && presentFamily.has_value();
		//return graphicsFamily.has_value() && presentFamily >= 0;
	}
};

struct Vertex
{
    float position[3];
    float color[3];
    float uv[2];
    float normal[3];

    static VkVertexInputBindingDescription getBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions()
    {
        std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, position);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, uv);

        attributeDescriptions[3].binding = 0;
        attributeDescriptions[3].location = 3;
        attributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[3].offset = offsetof(Vertex, normal);

        return attributeDescriptions;
    }
};

struct Mat4
{
    float elements[16];

    static Mat4 identity()
    {
        return Mat4{{
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        }};
    }

    static Mat4 rotationZ(float radians)
    {
        const float c = std::cos(radians);
        const float s = std::sin(radians);
        return Mat4{{
             c,    s, 0.0f, 0.0f,
            -s,    c, 0.0f, 0.0f,
          0.0f, 0.0f, 1.0f, 0.0f,
          0.0f, 0.0f, 0.0f, 1.0f
        }};
    }

    static Mat4 translation(float x, float y, float z)
    {
        return Mat4{{
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
               x,    y,    z, 1.0f
        }};
    }

    static Mat4 scale(float x, float y, float z)
    {
        return Mat4{{
               x, 0.0f, 0.0f, 0.0f,
            0.0f,    y, 0.0f, 0.0f,
            0.0f, 0.0f,    z, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        }};
    }

    static Mat4 orthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane)
    {
        return Mat4{{
            2.0f / (right - left), 0.0f, 0.0f, 0.0f,
            0.0f, 2.0f / (top - bottom), 0.0f, 0.0f,
            0.0f, 0.0f, -2.0f / (farPlane - nearPlane), 0.0f,
            -(right + left) / (right - left),
            -(top + bottom) / (top - bottom),
            -(farPlane + nearPlane) / (farPlane - nearPlane),
            1.0f
        }};
    }

    static Mat4 perspective(float fovRadians, float aspect, float nearPlane, float farPlane)
    {
        const float tanHalfFov = std::tan(fovRadians * 0.5f);

        return Mat4{{
            1.0f / (aspect * tanHalfFov), 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f / tanHalfFov, 0.0f, 0.0f,
            0.0f, 0.0f, farPlane / (nearPlane - farPlane), -1.0f,
            0.0f, 0.0f, (farPlane * nearPlane) / (nearPlane - farPlane), 0.0f
        }};
    }
};

inline Mat4 multiply(const Mat4& lhs, const Mat4& rhs)
{
    Mat4 result{};

    for (int row = 0; row < 4; ++row)
    {
        for (int column = 0; column < 4; ++column)
        {
            float value = 0.0f;
            for (int k = 0; k < 4; ++k)
            {
                value += lhs.elements[row * 4 + k] * rhs.elements[k * 4 + column];
            }
            result.elements[row * 4 + column] = value;
        }
    }

    return result;
}

inline Mat4 transpose(const Mat4& matrix)
{
    Mat4 result{};

    for (int row = 0; row < 4; ++row)
    {
        for (int column = 0; column < 4; ++column)
        {
            result.elements[row * 4 + column] = matrix.elements[column * 4 + row];
        }
    }

    return result;
}


struct UniformBufferObject
{
    Mat4 view;
    Mat4 projection;
    Mat4 viewProjection;
    float cameraPosition[4] = { 0.0f, 0.0f, 2.5f, 1.0f };
    float projectionParams[4] = { 1.0f, 0.57735026f, 0.1f, 10.0f };
    float lightDirection[4] = { -0.45f, -0.8f, -0.35f, 0.0f };
    float lightColor[4] = { 1.0f, 0.98f, 0.92f, 1.0f };
    float ambientColor[4] = { 0.22f, 0.24f, 0.3f, 1.0f };
    float debugOptions[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
};

struct MeshPushConstants
{
    Mat4 model;
    float baseColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
};

struct Material
{
    float baseColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    uint64_t baseColorTextureAssetId = 0;
    float useBaseColorTexture = 0.0f;
};

struct Transform
{
    float position[3] = { 0.0f, 0.0f, 0.0f };
    float rotationRadians = 0.0f;
    float scale[3] = { 1.0f, 1.0f, 1.0f };

    Mat4 toMatrix() const
    {
        return multiply(
            multiply(
                Mat4::scale(scale[0], scale[1], scale[2]),
                Mat4::rotationZ(rotationRadians)),
            Mat4::translation(position[0], position[1], position[2]));
    }
};

#endif // !ENGINE_COMMON_STRUCTS
