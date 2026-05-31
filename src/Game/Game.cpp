#include <SDL2/SDL_image.h>
#include <glm/glm.hpp>
#include <iostream>
#include <fstream>

#include "game.h"
#include "../logger/logger.h"
#include "../ecs/ecs.h"
#include "../components/transform_component.h"
#include "../components/rigid_body_component.h"
#include "../components/sprite_component.h"
#include "../components/animation_component.h"
#include "../systems/movement_system.h"
#include "../systems/render_system.h"
#include "../systems/animation_system.h"

game::game() 
{
    is_running = false;
	registry = std::make_unique<ecs::registry>();
	assets = std::make_unique<asset_store>();
    logger::log("Game constructor called!");
}

game::~game() 
{
    logger::log("Game destructor called!");
}

void game::initialize() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) 
	{
        logger::err("Error initializing SDL.");
        return;
    }

    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);
    window_width = 1280; // display_mode.w;
    window_height = 720; // display_mode.h;
    window = SDL_CreateWindow(
        nullptr,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        window_width,
    	window_height,
        SDL_WINDOW_SHOWN
    );
    if (!window) {
        logger::err("Error creating SDL Window.");
        return;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        logger::err("Error creating SDL Renderer.");
        return;
    }
    
	// SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
    is_running = true;
}

void game::process_input() 
{
    SDL_Event sdl_event;
    while (SDL_PollEvent(&sdl_event)) 
	{
        switch (sdl_event.type) 
		{
        case SDL_QUIT:
            is_running = false;
            break;
        case SDL_KEYDOWN:
            if (sdl_event.key.keysym.sym == SDLK_ESCAPE) 
			{
                is_running = false;
            }
            break;
        }
    }
}

void game::load_level(int level)
{
	// Add the systems that need to be processed in our game
	registry->add_system<movement_system>();
	registry->add_system<render_system>();
	registry->add_system<animation_system>();

	// Adding assets to the asset store
	assets->add_texture(renderer, "tank-image", "../assets/images/tank-panther-right.png");
	assets->add_texture(renderer, "truck-image", "../assets/images/truck-ford-right.png");
	assets->add_texture(renderer, "chopper-image", "../assets/images/chopper.png");
	assets->add_texture(renderer, "radar-image", "../assets/images/radar.png");
	assets->add_texture(renderer, "tilemap-image", "../assets/tilemaps/jungle.png");

	// Load the tilemap
	int tile_size = 32;
	double tile_scale = 2.0;
	int map_num_cols = 25;
	int map_num_rows = 20;

	std::fstream map_file;
	map_file.open("../assets/tilemaps/jungle.map");

	for (int y = 0; y < map_num_rows; y++)
	{
		for (int x = 0; x < map_num_cols; x++)
		{
			char ch;
			map_file.get(ch);
			int src_rect_y = std::atoi(&ch) * tile_size;
			map_file.get(ch);
			int src_rect_x = std::atoi(&ch) * tile_size;
			map_file.ignore();

			ecs::entity tile = registry->create_entity();
			tile.add_component<transform_component>(glm::vec2(x * (tile_scale * tile_size), y * (tile_scale * tile_size)), glm::vec2(tile_scale, tile_scale), 0.0);
			tile.add_component<sprite_component>("tilemap-image", tile_size, tile_size, 0, src_rect_x, src_rect_y);
		}
	}
	map_file.close();

    // Create an entity
	ecs::entity chopper = registry->create_entity();
	chopper.add_component<transform_component>(glm::vec2(10.0, 100.0), glm::vec2(1.0, 1.0), 0.0);
	chopper.add_component<rigid_body_component>(glm::vec2(0.0, 0.0));
	chopper.add_component<sprite_component>("chopper-image", 32, 32, 1);
	chopper.add_component<animation_component>(2, 15, true);

	ecs::entity radar = registry->create_entity();
	radar.add_component<transform_component>(glm::vec2(window_width - 74, 10.0), glm::vec2(1.0, 1.0), 0.0);
	radar.add_component<rigid_body_component>(glm::vec2(0.0, 0.0));
	radar.add_component<sprite_component>("radar-image", 64, 64, 2);
	radar.add_component<animation_component>(8, 5, true);

	ecs::entity tank = registry->create_entity();
	tank.add_component<transform_component>(glm::vec2(10.0, 10.0), glm::vec2(1.0, 1.0), 0.0);
	tank.add_component<rigid_body_component>(glm::vec2(30.0, 0.0));
	tank.add_component<sprite_component>("tank-image", 32, 32, 1);

	ecs::entity truck = registry->create_entity();
	truck.add_component<transform_component>(glm::vec2(10.0, 50.0), glm::vec2(1.0, 1.0), 0.0);
	truck.add_component<rigid_body_component>(glm::vec2(20.0, 0.0));
	truck.add_component<sprite_component>("truck-image", 32, 32, 2);
}

void game::setup() 
{
	load_level(1);
}

void game::update() 
{
    // If we are too fast, waste time until we reach the MILLISECS_PER_FRAME
    int time_to_wait = MILLISECS_PER_FRAME - (SDL_GetTicks() - millisecs_previous_frame);
    if (time_to_wait > 0 && time_to_wait <= MILLISECS_PER_FRAME) 
	{
        SDL_Delay(time_to_wait);
    }

    // Difference in ticks from last frame, converrted to seconds
    double delta_time = (SDL_GetTicks() - millisecs_previous_frame) / 1000.0;

    // store the "previous" frame time
    millisecs_previous_frame = SDL_GetTicks();

    // Invoke all the systems that need to update
	registry->get_system<movement_system>().update(delta_time);
	registry->get_system<animation_system>().update();

	// Update the registry to process the entities that are waiting to be created/deleted
	registry->update();
}

void game::render() 
{
    SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
    SDL_RenderClear(renderer);

    // Invoke all the systems that need to update
	registry->get_system<render_system>().update(renderer, assets);

    SDL_RenderPresent(renderer);
}

void game::run() 
{
    setup();
    while (is_running) 
	{
        process_input();
        update();
        render();
    }
}

void game::destroy() 
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}