#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 projection;
    mat4 viewProjection;
} ubo;

layout(push_constant) uniform MeshPushConstants {
    mat4 model;
} pushConstants;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inUv;
layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragUv;

void main() {
    gl_Position = ubo.viewProjection * pushConstants.model * vec4(inPosition, 1.0);
    fragColor = inColor;
    fragUv = inUv;
}
