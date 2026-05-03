#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 projection;
    mat4 viewProjection;
    vec4 cameraPosition;
    vec4 projectionParams;
    vec4 lightDirection;
    vec4 lightColor;
    vec4 ambientColor;
    vec4 debugOptions;
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
    vec4 worldPosition = pushConstants.model * vec4(inPosition, 1.0);
    vec3 viewPosition = worldPosition.xyz - ubo.cameraPosition.xyz;
    float viewDepth = -viewPosition.z;

    float aspectRatio = ubo.projectionParams.x;
    float tanHalfFov = ubo.projectionParams.y;
    float nearPlane = ubo.projectionParams.z;
    float farPlane = ubo.projectionParams.w;

    float clipX = viewPosition.x / (aspectRatio * tanHalfFov);
    float clipY = viewPosition.y / tanHalfFov;
    float clipZ = (farPlane / (farPlane - nearPlane)) * viewDepth -
                  ((farPlane * nearPlane) / (farPlane - nearPlane));
    float clipW = viewDepth;

    gl_Position = vec4(clipX, clipY, clipZ, clipW);
    fragColor = inColor;
    fragUv = inUv;
    fragNormal = normalize(mat3(pushConstants.model) * inNormal);
}
