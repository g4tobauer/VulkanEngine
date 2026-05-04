#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 projection;
    mat4 viewProjection;
    vec4 cameraPosition;
    vec4 projectionParams;
    vec4 cameraOptions;
    vec4 lightDirection;
    vec4 lightColor;
    vec4 ambientColor;
    vec4 debugOptions;
} ubo;

layout(push_constant) uniform MeshPushConstants {
    vec4 objectPosition;
    vec4 objectScale;
    vec4 objectRotation;
} pushConstants;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inUv;
layout(location = 3) in vec3 inNormal;
layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragUv;
layout(location = 2) out vec3 fragNormal;

void main() {
    float rotationCos = pushConstants.objectRotation.x;
    float rotationSin = pushConstants.objectRotation.y;

    vec3 scaledPosition = inPosition * pushConstants.objectScale.xyz;
    vec3 rotatedPosition = vec3(
        (rotationCos * scaledPosition.x) - (rotationSin * scaledPosition.y),
        (rotationSin * scaledPosition.x) + (rotationCos * scaledPosition.y),
        scaledPosition.z);

    vec3 worldPosition = rotatedPosition + pushConstants.objectPosition.xyz;
    vec3 viewPosition = worldPosition - ubo.cameraPosition.xyz;
    float viewDepth = -viewPosition.z;

    float aspectRatio = ubo.projectionParams.x;
    float tanHalfFov = ubo.projectionParams.y;
    float nearPlane = ubo.projectionParams.z;
    float farPlane = ubo.projectionParams.w;
    float orthoHalfHeight = max(ubo.cameraOptions.y, 0.001);
    float orthoHalfWidth = max(aspectRatio * orthoHalfHeight, 0.001);
    int projectionMode = int(ubo.cameraOptions.x + 0.5);

    if (projectionMode == 0) {
        float clipX = viewPosition.x / orthoHalfWidth;
        float clipY = viewPosition.y / orthoHalfHeight;
        float clipZ = clamp((viewDepth - nearPlane) / (farPlane - nearPlane), 0.0, 1.0);
        gl_Position = vec4(clipX, clipY, clipZ, 1.0);
    } else {
        float safeDepth = max(viewDepth, nearPlane + 0.001);
        float ndcX = viewPosition.x / ((aspectRatio * tanHalfFov) * safeDepth);
        float ndcY = viewPosition.y / (tanHalfFov * safeDepth);
        float ndcZ = clamp((safeDepth - nearPlane) / (farPlane - nearPlane), 0.0, 1.0);

        gl_Position = vec4(ndcX, ndcY, ndcZ, 1.0);
    }

    vec3 rotatedNormal = vec3(
        (rotationCos * inNormal.x) - (rotationSin * inNormal.y),
        (rotationSin * inNormal.x) + (rotationCos * inNormal.y),
        inNormal.z);
    fragColor = inColor;
    fragUv = inUv;
    fragNormal = normalize(rotatedNormal);
}
