#pragma once

#include <vector>

#include "../ecs/ecs.h"
#include "../components/box_collider_component.h"
#include "../components/transform_component.h"

class collision_system: public ecs::system
{
	public:
		collision_system()
		{
			require_component<transform_component>();
			require_component<box_collider_component>();
		}

		void update()
		{
			auto entities = get_system_entities();

			for (auto entity: entities)
			{
				auto& collider = entity.get_component<box_collider_component>();
				collider.is_colliding = false;
			}

			// Loop all the entities that the system is interested in
			for (auto i = entities.begin(); i != entities.end(); i++)
			{
				ecs::entity a = *i;
				auto& a_transform = a.get_component<transform_component>();
				auto& a_collider = a.get_component<box_collider_component>();

				// Loop all the entities that still need to be checked (to the right of i)
				for (auto j = i; j != entities.end(); j++)
				{
					ecs::entity b = *j;

					// Bypass if we are trying to test the same entity
					if (a == b) { continue; }

					auto& b_transform = b.get_component<transform_component>();
					auto& b_collider = b.get_component<box_collider_component>();
				
					// Perform the aabb collision check between entities a and b
					bool collision_happened = check_aabb_collision(
						a_transform.position.x + a_collider.offset.x,
						a_transform.position.y + a_collider.offset.y,
						a_collider.width,
						a_collider.height,
						b_transform.position.x + b_collider.offset.x,
						b_transform.position.y + b_collider.offset.y,
						b_collider.width,
						b_collider.height
					);

					if (collision_happened)
					{
						logger::log("Entity " + std::to_string(a.get_id()) + " is colliding with entity " + std::to_string(b.get_id()));
						a_collider.is_colliding = true;
						b_collider.is_colliding = true;
						// TODO: emit an event...
					}
				}
			}
		}

		bool check_aabb_collision(double a_x, double a_y, double a_w, double a_h, double b_x, double b_y, double b_w, double b_h)
		{
			return (
				a_x < b_x + b_w &&
				a_x + a_w > b_x &&
				a_y < b_y + b_h &&
				a_y + a_h > b_y
			);
		}
};