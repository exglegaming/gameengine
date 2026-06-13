#pragma once

#include <SDL2/SDL.h>
#include <memory>

#include "../ecs/ecs.h"
#include "../asset_store/asset_store.h"

const int FPS = 60;
const int MILLISECS_PER_FRAME = 1000 / FPS;

class game 
{
	private:
		bool is_running = false;
		bool is_debug_mode = false;
		int millisecs_previous_frame = 0;
		SDL_Window* window;
		SDL_Renderer* renderer;

		std::unique_ptr<ecs::registry> registry;
		std::unique_ptr<asset_store> assets;

	public:
		game();
		~game();
		void initialize();
		void run();
		void setup();
		void load_level(int level);
		void process_input();
		void update();
		void render();
		void destroy();

		int window_width;
		int window_height;
};
