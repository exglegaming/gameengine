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
        NULL,
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
	
	// Make Window windowed
	// SDL_SetWindowFullscreen(window, 0);
	// SDL_SetWindowSize(window, 1280, 720);
	// SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

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

void Game::setup() 
{
	// Add the systems that need to be processed in our game
	registry->add_system<MovementSystem>();
	registry->add_system<RenderSystem>();

	// Adding assets to the asset store
	asset_store->add_texture(renderer, "tank-image", "../assets/images/tank-panther-right.png");
	asset_store->add_texture(renderer, "truck-image", "../assets/images/truck-ford-right.png");

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