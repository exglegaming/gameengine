#pragma once

#include <glm/glm.hpp>

struct box_collider_component
{
	int width;
	int height;
	glm::vec2 offset;
	bool is_colliding = false;

	box_collider_component(int width = 0, int height = 0, glm::vec2 offset = glm::vec2(0))
	{
		this->width = width;
		this->height = height;
		this->offset = offset;
	}
};
