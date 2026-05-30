#ifndef MOVEMENTSYSTEM_H
#define MOVEMENTSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"

class MovementSystem : public System 
{
	public:
		MovementSystem() 
		{
			require_component<TransformComponent>();
			require_component<RigidBodyComponent>();
		}

		void update(double delta_time) 
		{
			// Loop all entities that the system is interested in
			for (auto entity: get_system_entities()) {
				// Upadate entity position based on its velocity
				auto& transform = entity.get_component<TransformComponent>();
				const auto rigidbody = entity.get_component<RigidBodyComponent>();

				transform.position.x += rigidbody.velocity.x * delta_time;
				transform.position.y += rigidbody.velocity.y * delta_time;
			}
		}
};

#endif