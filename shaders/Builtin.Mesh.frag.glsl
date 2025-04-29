#version 450

#extension GL_GOOGLE_include_directive: require

#include "Builtin.Common.glsl"
#include "Builtin.Bindless.common.glsl"

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec2 in_uv;
layout (location = 2) in vec3 in_normal;
layout (location = 3) in vec4 in_tangent;
layout (location = 4) in mat3 in_tbn;

layout (location = 0) out vec4 out_frag;

void main() {
    Material material = meshPushConstant.sceneDataBuffer.data.materials.data[meshPushConstant.materialId];

    vec4 diffuse = sampleTexture2dLinear(material.diffuseTexture, in_uv);
    if (diffuse.a < 0.1) {
        discard;
    }

    vec3 baseColor = material.baseColor.rgb * diffuse.rgb;

    vec3 normal = normalize(in_normal).rgb;
    if (in_tangent != vec4(0.0)) {
        normal = sampleTexture2dLinear(material.normalTexture, in_uv).rgb;
        normal = in_tbn * normalize(normal * 2.0 - 1.0);
        normal = normalize(normal);
    }

    vec3 diffuseColor = baseColor;
    float metallic = 0.0f;
    float roughness = 1.0f;
    vec3 f0 = vec3(0.0);

    vec3 cameraPos = meshPushConstant.sceneDataBuffer.data.cameraPos.xyz;
    vec3 fragPos = in_pos.xyz;
    vec3 n = normal;
    vec3 v = normalize(cameraPos - fragPos);

    vec3 frag_color = vec3(0.0);
    for (int i = 0; i < meshPushConstant.sceneDataBuffer.data.lightCount; i++) {

    }

    out_frag = vec4(1.0, 0.0, 0.0, 1.0f);
}