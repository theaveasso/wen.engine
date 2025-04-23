#extension GL_EXT_nonuniform_qualifier: enable

layout (set = 0, binding = 0) uniform texture2D textures[];
layout (set = 0, binding = 0) uniform texture2DMS texturesMS[];
layout (set = 0, binding = 0) uniform textureCube textureCubes[];
layout (set = 0, binding = 0) uniform texture2DArray textureArrays[];
layout (set = 0, binding = 1) uniform sampler samplers[];

#define NEAREST_SAMPLER_ID      0
#define LINEAR_SAMPLER_ID       1
#define SHADOW_MAP_SAMPLER_ID   2

vec4 samplerTexture2dNearest(uint texId, vec2 uv) {
    return texture(nonuniformEXT(sampler2D(textures[texId], samplers[NEAREST_SAMPLER_ID])), uv);
}

vec4 samplerTexture2dMSNearest(uint texId, ivec2 p, int s) {
    return texelFetch(nonuniformEXT(sampler2DMS(texturesMS[texId], samplers[NEAREST_SAMPLER_ID])), p, s);
}

vec4 samplerTexture2dLinear(uint texId, vec2 uv) {
    return texture(nonuniformEXT(sampler2D(textures[texId], samplers[LINEAR_SAMPLER_ID])), uv);
}

vec4 sampleTextureCubeNearest(uint texId, vec3 p) {
    return texture(nonuniformEXT(samplerCube(textureCubes[texId], samplers[NEAREST_SAMPLER_ID])), p);
}

vec4 samplerTextureCubeLinear(uint texId, vec3 p) {
    return texture(nonuniformEXT(samplerCube(textureCubes[texId], samplers[LINEAR_SAMPLER_ID])), p);
}

float sampleTextureArrayShadow(uint texId, vec4 p) {
    return texture(nonuniformEXT(sampler2DArrayShadow(textureArrays[texId], samplers[SHADOW_MAP_SAMPLER_ID])), p);
}

