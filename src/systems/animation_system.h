#pragma once

#include <SDL2/SDL.h>

#include "../ecs/ecs.h"
#include "../components/animation_component.h"
#include "../components/sprite_component.h"

class animation_system : public ecs::system
{
	public:
		animation_system()
		{
			require_component<sprite_component>();
			require_component<animation_component>();
		}

		void update()
		{
			for (auto entity: get_system_entities())
			{
				auto& animation = entity.get_component<animation_component>();
				auto& sprite = entity.get_component<sprite_component>();

				animation.current_frame = ((SDL_GetTicks() - animation.start_time) * animation.frame_speed_rate / 1000) % animation.num_frames;
				sprite.src_rect.x = animation.current_frame * sprite.width;
			}
		}
};
