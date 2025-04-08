#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vulkan.h>

#include "graphics/renderer/vulkan/vk_shader.hpp"
#include "privates/wen_pch.hpp"

/// Define vertex structure
struct WenVertex
{
	glm::vec3 position;
	glm::vec3 color;
};

/// Triangle vertex data
const std::vector<WenVertex> vertices = {
    {{0.5, -0.5f, 0.0}, {1.0f, 0.0f, 0.0f}},
    {{0.5, 0.5f, 0.0}, {0.0f, 1.0f, 0.0f}},
    {{-0.5, 0.5f, 0.0}, {0.0f, 0.0f, 1.0f}},
};

/**
 * @brief Helper function to create a VkShaderModule
 *
 * @param device
 * @param stage
 * @param filename
 */
VkShaderModule wen_load_shader_spirv(
    VkDevice              device,
    VkShaderStageFlagBits stage,
    const std::string    &filename);
