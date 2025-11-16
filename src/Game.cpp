#include "Game.h"
#include <iostream>

Game::Game() {
    std::cout << "Game Constructor Called" << std::endl;
}

Game::~Game() {
    std::cout << "Game Deconstructor Called" << std::endl;
}

void Game::Initialize() {
    // TODO...
}

void Game::Run() {
    while (true) {
        ProcessInput();
        Update();
        Render();
    }
}

void Game::ProcessInput() {
    // TODO...
}

void Game::Update() {
    // TODO...
}

void Game::Render() {
    // TODO...
}

void Game::Destroy() {
    // TODO...
}
