// g++ main.cpp -o prog `pkg-config --cflags --libs sdl3`
// ./prog

#include <SDL3/SDL.h>

int main(int argc, char const *argv[])
{
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Failed");
        return -1;
    }

    SDL_Window *window;
    window = SDL_CreateWindow("Aiky - SDL3", 320, 240, 0);

    SDL_Delay(5000);

    SDL_Quit();
    return 0;
}
