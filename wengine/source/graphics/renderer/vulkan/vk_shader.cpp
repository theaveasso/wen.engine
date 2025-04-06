#include "vk_shader.hpp"

#include "utils/shader_compiler.hpp"

static void
flush_builder_state(WenVkShaderBuilder *builder);

void
shader_ctx_init(WenVkShaderContext *shader_ctx) {
    shader_ctx->builder_shader.shaderc_compile_options =
        shaderc_compile_options_initialize();

    shaderc_compile_options_set_target_spirv(
        shader_ctx->builder_shader.shaderc_compile_options,
        shaderc_spirv_version_1_6);
    shader_ctx->builder_shader.compiler = shaderc_compiler_initialize();
}

void
shader_ctx_fini(
    VkDevice                  device,
    const WenVkShaderContext *shader_ctx,
    const WenVkEXTContext    *ext_ctx) {
    shaderc_compiler_release(shader_ctx->builder_shader.compiler);

    for (const auto &vert_shader : shader_ctx->vert_shaders) {
        ext_ctx->vkDestroyShaderEXT(device, vert_shader.shader, nullptr);
    }

    for (const auto &frag_shader : shader_ctx->frag_shaders) {
        ext_ctx->vkDestroyShaderEXT(device, frag_shader.shader, nullptr);
    }
}

void
shader_ctx_stage_shader(
    WenVkShaderContext              *shader_ctx,
    const std::filesystem::path     &filename,
    std::string_view                 name,
    std::span<VkDescriptorSetLayout> desc_set_layouts,
    std::span<VkPushConstantRange>   push_constant_ranges,
    VkShaderStageFlagBits            stage,
    VkShaderStageFlags               next_stage) {
    std::vector<uint32_t> shader_spv;

    bool success = compile_shader_spv(
        shader_ctx->builder_shader.compiler,
        shader_ctx->builder_shader.shaderc_compile_options,
        filename.string(),
        stage,
        &shader_spv);
    if (!success) {
        WEN_ERROR("failed compiling shader {}", filename.string());
        return;
    }

    VkShaderCreateInfoEXT shader_create_info = {
        VK_STRUCTURE_TYPE_SHADER_CREATE_INFO_EXT};
    shader_create_info.stage                  = stage;
    shader_create_info.nextStage              = next_stage;
    shader_create_info.codeType               = VK_SHADER_CODE_TYPE_SPIRV_EXT;
    shader_create_info.pName                  = "main";
    shader_create_info.setLayoutCount         = desc_set_layouts.size();
    shader_create_info.pSetLayouts            = desc_set_layouts.data();
    shader_create_info.pushConstantRangeCount = push_constant_ranges.size();
    shader_create_info.pPushConstantRanges    = push_constant_ranges.data();
    shader_create_info.pSpecializationInfo    = nullptr;

    shader_ctx->builder_shader.create_infos.push_back(shader_create_info);
    shader_ctx->builder_shader.spvs.push_back(shader_spv);
    shader_ctx->builder_shader.names.emplace_back(name.data());
    shader_ctx->builder_shader.paths.emplace_back(filename.string());
}

void
shader_ctx_commit_shader(
    VkDevice               device,
    WenVkShaderContext    *shader_ctx,
    const WenVkEXTContext *ext_ctx,
    WenVkShaderType        type) {

    std::vector<VkShaderEXT> shader_exts{};
    shader_exts.resize(shader_ctx->builder_shader.create_infos.size());

    for (uint32_t i = 0; i < shader_ctx->builder_shader.create_infos.size();
         ++i) {
        shader_ctx->builder_shader.create_infos[i].flags |=
            type == WenVkShaderType::LINKED
                ? VK_SHADER_CREATE_LINK_STAGE_BIT_EXT
                : 0;
        shader_ctx->builder_shader.create_infos[i].codeSize =
            shader_ctx->builder_shader.spvs[i].size() * sizeof(uint32_t);
        shader_ctx->builder_shader.create_infos[i].pCode =
            shader_ctx->builder_shader.spvs[i].data();
    }
    vk_check(ext_ctx->vkCreateShadersEXT(
        device,
        shader_ctx->builder_shader.create_infos.size(),
        shader_ctx->builder_shader.create_infos.data(),
        nullptr,
        shader_exts.data()));

    for (size_t i = 0; i < shader_exts.size(); ++i) {
        VkShaderEXT           shader_ext = shader_exts[i];
        std::string          &name       = shader_ctx->builder_shader.names[i];
        std::string          &path       = shader_ctx->builder_shader.paths[i];
        VkShaderCreateInfoEXT shader_create_info =
            shader_ctx->builder_shader.create_infos[i];

        WenVkShader s;
        s.name               = name;
        s.path               = path;
        s.shader             = shader_ext;
        s.shader_create_info = shader_create_info;
        s.stage              = shader_ctx->builder_shader.create_infos[i].stage;

        if (shader_ctx->builder_shader.create_infos[i].stage ==
            VK_SHADER_STAGE_VERTEX_BIT) {
            shader_ctx->vert_shaders.push_back(s);
        } else if (
            shader_ctx->builder_shader.create_infos[i].stage ==
            VK_SHADER_STAGE_FRAGMENT_BIT) {
            shader_ctx->frag_shaders.push_back(s);
        } else {
            WEN_ERROR(
                "shader stage {} not handled",
                WEN_CAST(
                    uint32_t,
                    shader_ctx->builder_shader.create_infos[i].stage));
        }
    }
    flush_builder_state(&shader_ctx->builder_shader);
}

void
shader_ctx_replace_shader(
    VkDevice               device,
    WenVkShaderContext    *shader_ctx,
    const WenVkEXTContext *ext_ctx) {
}

void
flush_builder_state(WenVkShaderBuilder *builder) {
    builder->create_infos.clear();
    builder->spvs.clear();
    builder->names.clear();
    builder->paths.clear();
}
