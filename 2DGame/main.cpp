// gcc main.cpp -o prog `pkg-config --cflags --libs sdl3 sdl3-image`
// ./prog

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

struct SDLState
{
    SDL_Window *window;
    SDL_Renderer *renderer;
};

void cleanup(SDLState &state);

int main(int argc, char const *argv[])
{
    SDLState state;
    // Checks if SDL initialized properly
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error initializing SDL3", nullptr);
        return 1;
    }

    // Varaible for the window
    SDL_Window *window;

    // Width and Heights for the window
    int width = 800;
    int height = 600;

    // Create the window with parameters
    state.window = SDL_CreateWindow("SDL3 Demo Game Tutorial", width, height, 0);

    if (!state.window) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error creating window", nullptr);
        cleanup(state);
        return 1;
    }

    // Create an event object
    SDL_Event event;

    // Create renderer
    state.renderer = SDL_CreateRenderer(state.window, nullptr);
    if (!state.renderer) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error creating renderer", state.window);
        return 1;
    }

    // Loads Assets
    SDL_Texture*idleTex = IMG_LoadTexture(state.renderer, "data/idle.png");

    // Game Loop
    bool running = true;
    while (running) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_EVENT_QUIT:
                running = false;
                break;
            }
        }

        // Perform drawing commands
        SDL_SetRenderDrawColor(state.renderer, 255,255,255,255);
        SDL_RenderClear(state.renderer);

        // Swap buffers and present
        SDL_RenderPresent(state.renderer);
    }

    cleanup(state);
    return 0;
}

void cleanup(SDLState &state)
{
    SDL_DestroyRenderer(state.renderer);
    SDL_DestroyWindow(state.window);
    SDL_Quit();
}