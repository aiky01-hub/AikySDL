// gcc main.cpp -o prog `pkg-config --cflags --libs sdl3 sdl3-image`
// ./prog

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

struct SDLState {
    SDL_Window* window;
    SDL_Renderer* renderer;
    int width, height, logW, logH;
};

bool initialize(SDLState& state);
void cleanup(SDLState& state);

int main(int argc, char const* argv[]) {
    SDLState state;
    state.width = 1600;
    state.height = 900;
    state.logW = 640;
    state.logH = 320;

    if (!initialize(state)) {
        return 1;
    }

    // Loads Assets
    SDL_Texture* idleTex = IMG_LoadTexture(state.renderer, "data/idle.png");
    SDL_SetTextureScaleMode(idleTex, SDL_SCALEMODE_NEAREST);

    // Game Data
    const bool* keys = SDL_GetKeyboardState(nullptr);
    float playerX = 0;
    const float floor = state.logH;
    uint16_t prevTime = SDL_GetTicks();

    // Game Loop
    bool running = true;
    while (running) {
        // Create an event object
        SDL_Event event{0};
        uint16_t nowTime = SDL_GetTicks();
        float deltaTime = (nowTime - prevTime) / 1000.0f;  // Convert to second
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_QUIT: {
                    running = false;
                    break;
                }
                case SDL_EVENT_WINDOW_RESIZED: {
                    state.width = event.window.data1;
                    state.height = event.window.data2;
                    break;
                }
            }
        }

        // Handle Movement
        float moveAmount = 0;
        if (keys[SDL_SCANCODE_A]) {
            moveAmount += -75.0f;
        }
        if (keys[SDL_SCANCODE_D]) {
            moveAmount += 75.0f;
        }
        playerX += moveAmount * deltaTime;

        // Perform drawing commands
        SDL_SetRenderDrawColor(state.renderer, 20, 10, 30, 255);
        SDL_RenderClear(state.renderer);

        const float spriteSize = 32;
        SDL_FRect src{
            .x = 0,
            .y = 0,
            .w = spriteSize,
            .h = spriteSize,
        };

        SDL_FRect dst{
            .x = playerX,
            .y = floor - spriteSize,
            .w = spriteSize,
            .h = spriteSize,
        };

        SDL_RenderTexture(state.renderer, idleTex, &src, &dst);

        // Swap buffers and present
        SDL_RenderPresent(state.renderer);
        prevTime = nowTime;
    }

    SDL_DestroyTexture(idleTex);
    cleanup(state);
    return 0;
}

void cleanup(SDLState& state) {
    SDL_DestroyRenderer(state.renderer);
    SDL_DestroyWindow(state.window);
    SDL_Quit();
}

bool initialize(SDLState& state) {
    bool initSuccess = true;
    // Checks if SDL initialized properly
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error initializing SDL3", nullptr);
        initSuccess = false;
    }

    // Create the window with parameters
    state.window = SDL_CreateWindow("SDL3 Demo Game Tutorial", state.width, state.height, SDL_WINDOW_RESIZABLE);

    if (!state.window) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error creating window", nullptr);
        cleanup(state);
        initSuccess = false;
    }

    // Create renderer
    state.renderer = SDL_CreateRenderer(state.window, nullptr);
    if (!state.renderer) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error creating renderer", state.window);
        cleanup(state);
        initSuccess = false;
    }

    SDL_SetRenderLogicalPresentation(state.renderer, state.logW, state.logH, SDL_LOGICAL_PRESENTATION_LETTERBOX);
    return initSuccess;
}