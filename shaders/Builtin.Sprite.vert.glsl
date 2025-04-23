#version 450

#extension GL_GOOGLE_include_directive: require
#extension GL_EXT_buffer_reference: require

layout (location = 0) out vec2 out_uv;
layout (location = 1) out vec4 out_color;
layout (location = 2) flat out uint texture_id;
layout (location = 3) flat out uint shader_id;

#include "Builtin.Sprite.common.glsl"

void main() {
    vec2 base_coords[6] = vec2[](
        vec2(0.0, 0.0),
        vec2(1.0, 0.0),
        vec2(0.0, 1.0),
        vec2(1.0, 0.0),
        vec2(1.0, 1.0),
        vec2(0.0, 1.0)
    );

    vec2 base_coord = base_coords[gl_VertexIndex % 6];

    SpriteDrawCommand command = pcs.drawBuffer.commands[gl_InstanceIndex];

    gl_Position = pcs.viewProj * command.transform * vec4(base_coord, 0.f, 1.f);
    out_uv = (1.f - base_coord) * command.uv0 + base_coord * command.uv1;
    out_color = command.color;
    texture_id = command.textureId;
    shader_id = command.shaderId;
}
