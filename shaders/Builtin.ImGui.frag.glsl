#version 460

#extension GL_GOOGLE_include_directive : require

#include "Builtin.Bindless.common.glsl"
#include "Builtin.ImGui.common.glsl"

// Explanation and parts of implementation adapted from DiligentEngine
// See here: https://github.com/DiligentGraphics/DiligentTools/blob/da116e30adff75ccdb33443d604ca6d153ee1589/Imgui/src/ImGuiDiligentRenderer.cpp#L39
// The original comment from DilligentEngine explaining what's going on is below:
//
// <START OF DILLIGENT_ENGINE COMMENT>
//
// Intentionally or not, all imgui examples render everything in sRGB space.
// Whether imgui expected it or not, the display engine then transforms colors to linear space
// https://stackoverflow.com/a/66401423/4347276
// We, however, (correctly) render everything in linear space letting the GPU to transform colors to sRGB,
// so that the display engine then properly shows them.
//
// As a result, there is a problem with alpha-blending: imgui performs blending directly in gamma-space, and
// gamma-to-linear conversion is done by the display engine:
//
//   Px_im = GammaToLinear(Src * A + Dst * (1 - A))                     (1)
//
// If we only convert imgui colors from sRGB to linear, we will be performing the following (normally)
// correct blending:
//
//   Px_dg = GammaToLinear(Src) * A + GammaToLinear(Dst) * (1 - A)      (2)
//
// However in case of imgui, this produces significantly different colors. Consider black background (Dst = 0):
//
//   Px_im = GammaToLinear(Src * A)
//   Px_dg = GammaToLinear(Src) * A
//
// We use the following equation that approximates (1):
//
//   Px_dg = GammaToLinear(Src * A) + GammaToLinear(Dst) * GammaToLinear(1 - A)  (3)
//
// Clearly (3) is not quite the same thing as (1), however it works surprisingly well in practice.
// Color pickers, in particular look properly.
//
// <END OF DILLIGENT_ENGINE COMMENT>

layout (location = 0) in vec4 in_color;
layout (location = 1) in vec2 in_uv;

layout (location = 0) out vec4 out_color;

// Note that approximate gamma-to-linear conversion pow(gamma, 2.2) produces
// considerably different colors.
float gamma_to_linear(float gamma) {
    return gamma < 0.04045 ?
    gamma / 12.92 :
    pow(max(gamma + 0.055, 0.0) / 1.055, 2.4);
}

void main()
{
    vec4 color = in_color * sampleTexture2dNearest(pcs.textureId, in_uv);
    color.rgb *= color.a;

    if (pcs.textureIsSRGB != 0) {
        // SRGBA to linear
        color.r = gamma_to_linear(color.r);
        color.g = gamma_to_linear(color.g);
        color.b = gamma_to_linear(color.b);
        color.a = 1.0 - gamma_to_linear(1 - color.a);
    }

    out_color = color;
}
