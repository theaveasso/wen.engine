#version 450

#extension GL_GOOGLE_include_directive: require
#extension GL_EXT_buffer_reference: require

#include "Builtin.Sprite.common.glsl"
#include "Builtin.Bindless.common.glsl"

layout (location = 0) in vec2 in_uv;
layout (location = 1) in vec4 in_color;
layout (location = 2) flat in uint textureId;
layout (location = 3) flat in uint shaderId;

layout (location = 0) out vec4 out_color;

#define SPRITE_SHADER_ID    0
#define TEXTURE_SHADER_ID   1

void main() {
    vec4 tex_color = samplerTexture2dNearest(textureId, in_uv);
    if (shaderId == TEXTURE_SHADER_ID) {
        tex_color = vec4(1.0, 1.0, 1.0, tex_color.r);
    }

    if (tex_color.a < 0.1) {
        discard;
    }

    out_color = in_color * tex_color;
}

