#ifndef MOVEMENTSYSTEM_H
#define MOVEMENTSYSTEM_H

#include "../ecs/ecs.h"
#include "../components/transform_component.h"
#include "../components/rigid_body_component.h"

class movement_system : public ecs::system 
{
	public:
		movement_system() 
		{
			require_component<transform_component>();
			require_component<rigid_body_component>();
		}

		void update(double delta_time) 
		{
			// Loop all entities that the system is interested in
			for (auto entity: get_system_entities()) {
				// Upadate entity position based on its velocity
				auto& transform = entity.get_component<transform_component>();
				const auto rigidbody = entity.get_component<rigid_body_component>();

				transform.position.x += rigidbody.velocity.x * delta_time;
				transform.position.y += rigidbody.velocity.y * delta_time;
			}
		}
};

#endif