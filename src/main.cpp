#include "./game/game.h"


int main(int argc, char* argv[]) 
{
    game game;

    game.initialize();
    game.run();
    game.destroy();

    return 0;
}