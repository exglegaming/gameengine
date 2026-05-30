#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include <SDL2/SDL.h>

class RenderSystem : public System 
{
	public:
		RenderSystem() 
		{
			require_component<TransformComponent>();
			require_component<SpriteComponent>();
		}

		void update(SDL_Renderer* renderer) 
		{
			// Loop all entities that the system is interested in
			for (auto entity: get_system_entities()) {
				// Upadate entity position based on its velocity
				const auto transform = entity.get_component<TransformComponent>();
				const auto sprite = entity.get_component<SpriteComponent>();

				// TODO: Draw the PNG texture
			}
				
		}
};

#endif