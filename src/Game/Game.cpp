#include "Game.h"
#include "../Logger/Logger.h"
#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Systems/MovementSystem.h"
#include "../Systems/RenderSystem.h"
#include <SDL2/SDL_image.h>
#include <glm/glm.hpp>
#include <iostream>
#include <fstream>

Game::Game() 
{
    is_running = false;
	registry = std::make_unique<Registry>();
	asset_store = std::make_unique<AssetStore>();
    Logger::log("Game constructor called!");
}

Game::~Game() 
{
    Logger::log("Game destructor called!");
}

void Game::initialize() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) 
	{
        Logger::err("Error initializing SDL.");
        return;
    }

    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);
    window_width = display_mode.w;
    window_height = display_mode.h;
    window = SDL_CreateWindow(
        nullptr,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1280,
    	720,
        SDL_WINDOW_SHOWN
    );
    if (!window) {
        Logger::err("Error creating SDL Window.");
        return;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        Logger::err("Error creating SDL Renderer.");
        return;
    }
    
	// SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
    is_running = true;
}

void Game::process_input() 
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

void Game::load_level(int level)
{
	// Add the systems that need to be processed in our game
	registry->add_system<MovementSystem>();
	registry->add_system<RenderSystem>();

	// Adding assets to the asset store
	asset_store->add_texture(renderer, "tank-image", "../assets/images/tank-panther-right.png");
	asset_store->add_texture(renderer, "truck-image", "../assets/images/truck-ford-right.png");
	asset_store->add_texture(renderer, "tilemap-image", "../assets/tilemaps/jungle.png");

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

			Entity tile = registry->create_entity();
			tile.add_component<TransformComponent>(glm::vec2(x * (tile_scale * tile_size), y * (tile_scale * tile_size)), glm::vec2(tile_scale, tile_scale), 0.0);
			tile.add_component<SpriteComponent>("tilemap-image", tile_size, tile_size, src_rect_x, src_rect_y);
		}
	}
	map_file.close();

    // Create an entity
	Entity tank = registry->create_entity();
	tank.add_component<TransformComponent>(glm::vec2(10.0, 10.0), glm::vec2(1.0, 1.0), 0.0);
	tank.add_component<RigidBodyComponent>(glm::vec2(40.0, 0.0));
	tank.add_component<SpriteComponent>("tank-image", 32, 32);

	Entity truck = registry->create_entity();
	truck.add_component<TransformComponent>(glm::vec2(50.0, 100.0), glm::vec2(1.0, 1.0), 0.0);
	truck.add_component<RigidBodyComponent>(glm::vec2(0.0, 50.0));
	truck.add_component<SpriteComponent>("truck-image", 32, 32);

}

void Game::setup() 
{
	load_level(1);
}

void Game::update() 
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
	registry->get_system<MovementSystem>().update(delta_time);

	// Update the registry to process the entities that are waiting to be created/deleted
	registry->update();
}

void Game::render() 
{
    SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
    SDL_RenderClear(renderer);

    // Invoke all the systems that need to update
	registry->get_system<RenderSystem>().update(renderer, asset_store);

    SDL_RenderPresent(renderer);
}

void Game::run() 
{
    setup();
    while (is_running) 
	{
        process_input();
        update();
        render();
    }
}

void Game::destroy() 
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}