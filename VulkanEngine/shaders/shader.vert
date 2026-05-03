#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 projection;
    mat4 viewProjection;
    vec4 lightDirection;
    vec4 lightColor;
    vec4 ambientColor;
} ubo;

layout(push_constant) uniform MeshPushConstants {
    mat4 model;
} pushConstants;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inUv;
layout(location = 3) in vec3 inNormal;
layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragUv;
layout(location = 2) out vec3 fragNormal;

void main() {
    gl_Position = ubo.viewProjection * pushConstants.model * vec4(inPosition, 1.0);
    fragColor = inColor;
    fragUv = inUv;
    fragNormal = normalize(mat3(pushConstants.model) * inNormal);
}
