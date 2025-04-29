#version 450

#extension GL_GOOGLE_include_directive: require

#include "Builtin.Common.glsl"

layout (location = 0) out vec3 out_pos;
layout (location = 1) out vec2 out_uv;
layout (location = 2) out vec3 out_normal;
layout (location = 3) out vec4 out_tangent;
layout (location = 4) out mat3 out_tbn;

void main() {
    Vertex vert = meshPushConstant.vertexBuffer.vertices[gl_VertexIndex];

    vec4 worldPos = meshPushConstant.transformMatrix * vec4(vert.position, 1.0f);

    gl_Position = meshPushConstant.sceneDataBuffer.data.viewProj * worldPos;

    out_pos = worldPos.xyz;
    out_uv = vec2(vert.uv_x, vert.uv_y);
    out_normal = mat3(transpose(inverse(meshPushConstant.transformMatrix))) * vert.normal;

    vec3 T = normalize(vec3(meshPushConstant.transformMatrix * vert.tangent));
    vec3 N = normalize(out_normal);
    vec3 B = cross(N, T) * vert.tangent.w;

    out_tbn = mat3(T, B, N);
}