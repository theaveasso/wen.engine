#ifndef BUILTIN_COMMON_GLSL
#define BUILTIN_COMMON_GLSL

#extension GL_EXT_buffer_reference: require
#extension GL_EXT_scalar_block_layout: require

#define WVK_SMOOTH_SHADOW
#define WVK_CASCADES_SHADOW_COUNT 3

/// Use for scalar-block aligned buffer references
#define WVK_BUFFER_REF layout (buffer_reference, scalar) readonly buffer

/// Use for std430 buffer references
#define WVK_BUFFER_REF_STD430 layout (buffer_reference, std430) readonly buffer

/// Use for push_constant references
#define WVK_PUSH_CONSTANT layout (push_constant, scalar) uniform

/// Vertex structure
struct Vertex {
    vec3 position;
    float uv_x;
    vec3 normal;
    float uv_y;
    vec4 tangent;
};

WVK_BUFFER_REF_STD430 VertexBuffer {
    Vertex vertices[];
} vertexBuffer;

/// Light structure
struct Light {
    vec3 position;
    uint type;
    vec3 direction;
    float range;
    vec3 color;
    float intensity;
    vec2 scaleOffset; // spot light only
    uint shadowMapId;
    float unused;
};

WVK_BUFFER_REF LightDataBuffer {
    Light data[];
} lightDataBuffer;

/// Material structure
struct Material {
    vec4 baseColor;
    vec4 metallicRoughnessEmissive;
    uint diffuseTexture;
    uint normalTexture;
    uint metallicRoughnessTexture;
    uint emissizeTexture;
};

WVK_BUFFER_REF MaterialDataBuffer {
    Material data[];
} materialDataBuffer;

/// Scene data structure
struct SceneData {
    // camera
    mat4 view;
    mat4 proj;
    mat4 viewProj;
    vec4 cameraPos;

    // ambient
    vec3 ambientColor;
    float ambientIntensity;

    // fog
    vec3 fogColor;
    float fogIntensity;

    // csm data
    vec4 cascadeFarPlaneZs;
    mat4 cascadeSpaceTMs[3];
    uvec4 cascadeShadowMapId;

    float pointLightFarPlane;

    // lighting
    LightDataBuffer lights;
    int lightCount;
    int sunlightIndex; // if -1, there no sun

    // materials
    MaterialDataBuffer materials;
};

WVK_BUFFER_REF SceneDataBuffer {
    SceneData data;
} sceneDataBuffer;

WVK_PUSH_CONSTANT MeshPushConstant {
    mat4 transformMatrix; ///< Precompute viewProj * model
    SceneDataBuffer sceneDataBuffer;
    VertexBuffer vertexBuffer;
    uint materialId;
    uint padding;
} meshPushConstant;

#endif // BUILTIN_COMMON_GLSL
