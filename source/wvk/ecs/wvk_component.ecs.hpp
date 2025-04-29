#pragma once

#include "wvk_ecs.hpp"

#include "wvk/core/wvk_math.hpp"
#include <glm/mat4x4.hpp>

namespace wvk::ecs
{
struct TransformComponent
{
	core::Transform transform;        // local transform (relative to parent)
	glm::mat4       worldTransform = glm::mat4(1.0f);
};

struct MovementComponent
{
	glm::vec3 kinematicVelocity;
	glm::vec3 maxSpeed;

	glm::vec3 prevFramePosition;
	glm::vec3 effectiveVelocity;

	glm::quat startHeading;
	glm::quat targetHeading;
	float     rotationTime     = 0.0f;
	float     rotationProgress = 0.0f;
};

struct SpriteComponent
{
	int                   z = 0;
	std::filesystem::path spritePath;
};

struct SpriteAnimationComponent
{
	std::string defaultAnimationName = "idle";
	std::string animationDataTag;
};
}        // namespace wvk::ecs