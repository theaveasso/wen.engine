#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <vector>

struct WenVkPipeline
{
	VkPipeline       pipeline;
	VkPipelineLayout pipeline_layout;
};

struct WenWorldData
{
	glm::mat4 view{1.0f};
	glm::mat4 projection{1.0f};
	glm::vec4 camera_position{};
};
