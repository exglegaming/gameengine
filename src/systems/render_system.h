#pragma once

#include <SDL2/SDL.h>
#include <vector>
#include <algorithm>

#include "../ecs/ecs.h"
#include "../components/transform_component.h"
#include "../components/sprite_component.h"
#include "../asset_store/asset_store.h"

class render_system : public ecs::system 
{
	public:
		render_system() 
		{
			require_component<transform_component>();
			require_component<sprite_component>();
		}

		void update(SDL_Renderer* renderer, std::unique_ptr<asset_store>& assets) 
		{
			// Sort all the entities of our system by the z-index
			std::vector<ecs::entity> z_index_entities = get_system_entities();
			std::sort(z_index_entities.begin(), z_index_entities.end(), [](const ecs::entity& entity_a, const ecs::entity& entity_b) {
				const auto sprite_a = entity_a.get_component<sprite_component>();
				const auto sprite_b = entity_b.get_component<sprite_component>();
				return sprite_a.z_index < sprite_b.z_index;
			});

			// Loop all entities that the system is interested in
			for (auto entity: z_index_entities) {
				// Update entity position based on its velocity
				const auto transform = entity.get_component<transform_component>();
				const auto sprite = entity.get_component<sprite_component>();

				// Set the source rectangle of our original sprite texture
				SDL_Rect src_rect = sprite.src_rect;

				// Set the destination rectangle with the x, y positiong to be rendered
				SDL_Rect dest_rect = {
					static_cast<int>(transform.position.x),
					static_cast<int>(transform.position.y),
					static_cast<int>(sprite.width * transform.scale.x),
					static_cast<int>(sprite.height * transform.scale.y)
				};

				SDL_RenderCopyEx(
					renderer,
					assets->get_texture(sprite.asset_id),
					&src_rect,
					&dest_rect,
					transform.rotation,
					NULL,
					SDL_FLIP_NONE
				);

				// TODO: Draw the PNG texture
			}
				
		}
};
