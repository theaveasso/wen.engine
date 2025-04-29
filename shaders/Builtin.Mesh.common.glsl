#extension GL_EXT_buffer_reference: require
#extension GL_EXT_scalar_block_layout: require

#include "Builtin.common.glsl"

layout (push_constant, scalar) uniform MeshPushConstant {
    mat4 transform;
    SceneDataBuffer sceneData;
} meshPC;