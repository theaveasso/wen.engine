#version 450

#extension GL_GOOGLE_include_directive: require

#include "Builtin.ImGui.common.glsl"

layout (location = 0) out vec4 out_color;
layout (location = 1) out vec2 out_uv;

void main() {
    const VertexFormat v = pcs.vertexBuffer.vertices[gl_VertexIndex];
    const vec4 color = unpackUnorm4x8(v.color);
    out_color = color;
    out_uv = v.uv;

    gl_Position = vec4(v.position * pcs.scale + pcs.translate, 0.0, 1.0);
}