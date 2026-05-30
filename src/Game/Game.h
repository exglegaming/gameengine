#ifndef GAME_H
#define GAME_H

#include "../ECS/ECS.h"
#include "../AssetStore/AssetStore.h"
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

	std::unique_ptr<Registry> registry;
	std::unique_ptr<AssetStore> asset_store;

public:
    Game();
    ~Game();
    void initialize();
    void run();
    void setup();
    void process_input();
    void update();
    void render();
    void destroy();

    int window_width;
    int window_height;
};

#endif