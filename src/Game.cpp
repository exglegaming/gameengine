#include "Game.h"
#include <SDL2/SDL.h>
#include <iostream>

Game::Game() {
    std::cout << "Game Constructor Called" << std::endl;
}

Game::~Game() {
    std::cout << "Game Deconstructor Called" << std::endl;
}

void Game::Initialize() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        std::cerr << "Error initializing SDL." << std::endl;
        return;
    }
    
    SDL_Window* window = SDL_CreateWindow(
        "Game Engine", 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED, 
        800,
        600,
        SDL_WINDOW_SHOWN
    );
    if (!window) {
        std::cerr << "Error creating SDL Window." << std::endl;
        return;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        std::cerr << "Error creating SDL Renderer." << std::endl;
        return;
    }
}

void Game::ProcessInput() {
    
}

void Game::Update() {
    
}

void Game::Render() {
    
}

void Game::Run() {
    // while (true) {
    //     ProcessInput();
    //     Update();
    //     Render();
    // }
}

void Game::Destroy() {
    
}
