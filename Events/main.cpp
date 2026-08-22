// gcc main.cpp -o prog `pkg-config --cflags --libs sdl3`
// ./prog

#include <SDL3/SDL.h>

int main(int argc, char const *argv[])
{
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Failed");
        return -1;
    }

    SDL_Window *window;
    window = SDL_CreateWindow("Aiky-Events", 320, 240, 0);

    SDL_Event event;

    bool running = true;
    while (running) {

        // Event handling loop
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_EVENT_QUIT:
                running = false;
                break;

            case SDL_EVENT_KEY_DOWN:
                SDL_Log("A key was pressed: %d", event.key.key);
                break;
            }
        }

        // Application / Game Logic loop
    }

    SDL_Quit();
    return 0;
}
