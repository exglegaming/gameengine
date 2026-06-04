#pragma once

#include <SDL2/SDL.h>

#include "../ecs/ecs.h"
#include "../components/box_collider_component.h"
#include "../components/transform_component.h"

class debug_render_system: public ecs::system
{
	public:
		debug_render_system()
		{
			require_component<transform_component>();
			require_component<box_collider_component>();
		}

		void update(SDL_Renderer* renderer)
		{
			for (auto entity: get_system_entities())
			{
				const auto& transform = entity.get_component<transform_component>();
				const auto& collider = entity.get_component<box_collider_component>();

				SDL_Rect rect = {
					static_cast<int>(transform.position.x + collider.offset.x),
					static_cast<int>(transform.position.y + collider.offset.y),
					static_cast<int>(collider.width),
					static_cast<int>(collider.height)
				};
				
				// Set rectangle draw color based on collision
				if (collider.is_colliding)
				{
					SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
					SDL_RenderDrawRect(renderer, &rect);
				}
				else
				{
					SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
					SDL_RenderDrawRect(renderer, &rect);
				}
			}
		}
};