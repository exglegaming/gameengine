#include "Game.h"
#include "../Logger/Logger.h"
#include "../ECS/ECS.h"
#include <SDL2/SDL_image.h>
#include <glm/glm.hpp>
#include <iostream>

Game::Game() 
{
    isRunning = false;
	registry = new Registry();
    Logger::Log("Game Constructor Called");
}

Game::~Game() 
{
    Logger::Log("Game Deconstructor Called");
}

void Game::Initialize() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) 
	{
        Logger::Err("Error initializing SDL.");
        return;
    }

    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);
    windowWidth = displayMode.w;
    windowHeight = displayMode.h;

    window = SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1280,
        720,
        SDL_WINDOW_SHOWN
    );
    if (!window) {
        Logger::Err("Error creating SDL Window.");
        return;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        Logger::Err("Error creating SDL Renderer.");
        return;
    }
    // SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
	
	// Make Window windowed
	// SDL_SetWindowFullscreen(window, 0);
	// SDL_SetWindowSize(window, 1280, 720);
	// SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    isRunning = true;
}

void Game::ProcessInput() 
{
    SDL_Event sdlEvent;
    while (SDL_PollEvent(&sdlEvent)) 
	{
        switch (sdlEvent.type) 
		{
        case SDL_QUIT:
            isRunning = false;
            break;
        case SDL_KEYDOWN:
            if (sdlEvent.key.keysym.sym == SDLK_ESCAPE) 
			{
                isRunning = false;
            }
            break;
        }
    }
}

void Game::Setup() 
{
    // Create some entities
	Entity tank = registry->CreateEntity();
	Entity truck = registry->CreateEntity();
	
}

void Game::Update() 
{
    // If we are too fast, waste time until we reach the MILLISECS_PER_FRAME
    int timeToWait = MILLISECS_PER_FRAME - (SDL_GetTicks() - millisecsPreviousFrame);
    if (timeToWait > 0 && timeToWait <= MILLISECS_PER_FRAME) 
	{
        SDL_Delay(timeToWait);
    }

    // Difference in ticks from last frame, converrted to seconds
    double deltaTime = (SDL_GetTicks() - millisecsPreviousFrame) / 1000.0;

    // store the "previous" frame time
    millisecsPreviousFrame = SDL_GetTicks();

    // TODO:
	// MovementSystem.Update();
	// CollisionSystem.Update();
	// DamageSystem.Update();
}

void Game::Render() 
{
    SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
    SDL_RenderClear(renderer);

    // TODO: Render game objects...

    SDL_RenderPresent(renderer);
}

void Game::Run() 
{
    Setup();
    while (isRunning) 
	{
        ProcessInput();
        Update();
        Render();
    }
}

void Game::Destroy() 
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}