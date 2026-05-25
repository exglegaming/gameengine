#pragma once

#include "../ECS/ECS.h"
#include <SDL2/SDL.h>

const int FPS = 60;
const int MILLISECS_PER_FRAME = 1000 / FPS;

class Game 
{
private:
    bool is_running;
    int millisecs_previous_frame = 0;
    SDL_Window* window;
    SDL_Renderer* renderer;

	Registry* registry;

public:
    Game();
    ~Game();
    void Initialize();
    void Run();
    void Setup();
    void ProcessInput();
    void Update();
    void Render();
    void Destroy();

    int window_width;
    int window_height;
};