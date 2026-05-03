#version 450

layout(set = 1, binding = 0) uniform sampler2D baseColorTexture;

layout(push_constant) uniform MeshPushConstants {
    mat4 model;
    vec4 baseColor;
} pushConstants;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragUv;

layout(location = 0) out vec4 outColor;

void main() {
    vec4 sampledColor = texture(baseColorTexture, fragUv);
    outColor = vec4(fragColor, 1.0) * pushConstants.baseColor * sampledColor;
}
