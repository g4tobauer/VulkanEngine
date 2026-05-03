#version 450

layout(set = 1, binding = 0) uniform sampler2D baseColorTexture;

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
    vec4 baseColor;
} pushConstants;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragUv;
layout(location = 2) in vec3 fragNormal;

layout(location = 0) out vec4 outColor;

void main() {
    vec4 sampledColor = texture(baseColorTexture, fragUv);
    vec3 normal = normalize(fragNormal);
    vec3 lightDir = normalize(-ubo.lightDirection.xyz);
    float diffuse = max(dot(normal, lightDir), 0.0);
    vec3 lighting = ubo.ambientColor.xyz + (ubo.lightColor.xyz * diffuse);
    vec3 albedo = fragColor * pushConstants.baseColor.rgb * sampledColor.rgb;
    outColor = vec4(albedo * lighting, pushConstants.baseColor.a * sampledColor.a);
}
