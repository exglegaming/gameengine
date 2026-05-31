#ifndef RIGIDBODYCOMPONENT_H
#define RIGIDBODYCOMPONENT_H

#include <glm/glm.hpp>

struct rigid_body_component
{
	glm::vec2 velocity;

	rigid_body_component(glm::vec2 velocity = glm::vec2(0.0, 0.0))
	{
		this->velocity = velocity;
	}
};

#endif