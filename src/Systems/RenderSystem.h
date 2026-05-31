#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include "../AssetStore/AssetStore.h"
#include <SDL2/SDL.h>
#include <vector>
#include <algorithm>

class RenderSystem : public System 
{
	public:
		RenderSystem() 
		{
			require_component<TransformComponent>();
			require_component<SpriteComponent>();
		}

		void update(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& asset_store) 
		{
			// Sort all the entities of our system by the z-index
			std::vector<Entity> z_index_entities = get_system_entities();
			std::sort(z_index_entities.begin(), z_index_entities.end(), [](const Entity& entity_a, const Entity& entity_b) {
				const auto sprite_a = entity_a.get_component<SpriteComponent>();
				const auto sprite_b = entity_b.get_component<SpriteComponent>();
				return sprite_a.z_index < sprite_b.z_index;
			});

			// Loop all entities that the system is interested in
			for (auto entity: z_index_entities) {
				// Update entity position based on its velocity
				const auto transform = entity.get_component<TransformComponent>();
				const auto sprite = entity.get_component<SpriteComponent>();

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
					asset_store->get_texture(sprite.asset_id),
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

#endif