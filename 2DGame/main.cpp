// gcc main.cpp -o prog `pkg-config --cflags --libs sdl3 sdl3-image`
// ./prog

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include <array>
#include <glm/glm.hpp>
#include <iostream>
#include <string>
#include <vector>

#include "gameobject.h"

struct SDLState {
    SDL_Window* window;
    SDL_Renderer* renderer;
    int width, height, logW, logH;
    const bool* keys;

    SDLState() : keys(SDL_GetKeyboardState(nullptr)) {
    }
};

const size_t Layer_IDX_LEVEL = 0;
const size_t Layer_IDX_CHARACTERS = 1;
const int MAP_ROWS = 5;
const int MAP_COLS = 50;
const int TILE_SIZE = 32;

struct GameState {
    std::array<std::vector<GameObject>, 2> layers;
    int playerIndex;

    GameState() {
        playerIndex = 0;
    }
};

// Keep track of all the pointers, integers, sound files, textures etc.
struct Resources {
    const int ANIM_PLAYER_IDLE = 0;
    const int ANIM_PLAYER_RUN = 1;
    std::vector<Animation> playerAnims;

    std::vector<SDL_Texture*> textures;
    SDL_Texture *texIdle, *texRun, *texBrick, *texGrass, *texGround, *texPanel;

    SDL_Texture* loadTexture(SDL_Renderer* renderer, const std::string& filepath) {
        // Loads Assets
        SDL_Texture* tex = IMG_LoadTexture(renderer, filepath.c_str());
        SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST);
        textures.push_back(tex);
        return tex;
    }

    void load(SDLState& state) {
        playerAnims.resize(5);
        playerAnims[ANIM_PLAYER_IDLE] = Animation(8, 1.6f);
        playerAnims[ANIM_PLAYER_RUN] = Animation(4, 0.5f);

        texIdle = loadTexture(state.renderer, "data/idle.png");
        texRun = loadTexture(state.renderer, "data/run.png");
        texBrick = loadTexture(state.renderer, "data/tiles/brick.png");
        texGrass = loadTexture(state.renderer, "data/tiles/grass.png");
        texGround = loadTexture(state.renderer, "data/tiles/ground.png");
        texPanel = loadTexture(state.renderer, "data/tiles/panel.png");
    }

    void unload() {
        for (SDL_Texture* tex : textures) {
            SDL_DestroyTexture(tex);
        }
    }
};

bool initialize(SDLState& state);
void cleanup(SDLState& state);
void drawObject(const SDLState& state, GameState& gs, GameObject& obj, float deltaTime);
void update(const SDLState& state, GameState& gs, Resources& res, GameObject& obj, float deltaTime);
void createTiles(const SDLState& state, GameState& gs, const Resources& res);

int main(int argc, char const* argv[]) {
    SDLState state;
    state.width = 1600;
    state.height = 900;
    state.logW = 640;
    state.logH = 320;

    if (!initialize(state)) {
        return 1;
    }

    // Load Game assets
    Resources res;
    res.load(state);

    // Game Data
    GameState gs;
    createTiles(state, gs, res);

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

        // update all objects
        for (auto& layer : gs.layers) {
            for (GameObject& obj : layer) {
                update(state, gs, res, obj, deltaTime);
                if (obj.currentAnimation != -1) {
                    obj.animations[obj.currentAnimation].step(deltaTime);
                }
            }
        }

        // Perform drawing commands
        SDL_SetRenderDrawColor(state.renderer, 20, 10, 30, 255);
        SDL_RenderClear(state.renderer);

        // Draw all objects
        for (auto& layer : gs.layers) {
            for (GameObject& obj : layer) {
                drawObject(state, gs, obj, deltaTime);
            }
        }

        // Swap buffers and present
        SDL_RenderPresent(state.renderer);
        prevTime = nowTime;
    }

    res.unload();
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

void drawObject(const SDLState& state, GameState& gs, GameObject& obj, float deltaTime) {
    const float spriteSize = 32;
    float srcX = obj.currentAnimation != -1
                     ? obj.animations[obj.currentAnimation].currentFrame() * spriteSize  // This will return where in the sprite sheet should the renderer draw from
                     : 0.0f;

    // Source of Texture
    SDL_FRect src{
        .x = srcX,
        .y = 0,
        .w = spriteSize,
        .h = spriteSize,
    };

    // Destination of the texture within the game frame;
    SDL_FRect dst{
        .x = obj.position.x,
        .y = obj.position.y,
        .w = spriteSize,
        .h = spriteSize,
    };

    SDL_FlipMode flipMode = obj.direction == -1 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

    SDL_RenderTextureRotated(state.renderer, obj.texture, &src, &dst, 0, nullptr, flipMode);
}

void update(const SDLState& state, GameState& gs, Resources& res, GameObject& obj, float deltaTime) {
    if (obj.type == ObjectType::player) {
        float currentDirection = 0;
        if (state.keys[SDL_SCANCODE_D]) {
            currentDirection += 1;
        }
        if (state.keys[SDL_SCANCODE_A]) {
            currentDirection += -1;
        }
        if (currentDirection) {
            obj.direction = currentDirection;
        }

        switch (obj.data.player.state) {
            case PlayerState::idle: {
                if (currentDirection) {
                    obj.data.player.state = PlayerState::running;
                    obj.texture = res.texRun;
                    obj.currentAnimation = res.ANIM_PLAYER_RUN;
                } else {
                    if (obj.velocity.x) {
                        const float factor = obj.velocity.x > 0 ? -1.5f : -1.5f;
                        float amount = factor * obj.acceleration.x * deltaTime;
                        if (std::abs(obj.velocity.x) < std::abs(amount)) {
                            obj.velocity.x = 0;
                        } else {
                            obj.velocity.x += amount;
                        }
                    }
                }
                break;
            }
            case PlayerState::running: {
                if (!currentDirection) {
                    obj.data.player.state = PlayerState::idle;
                    obj.texture = res.texIdle;
                    obj.currentAnimation = res.ANIM_PLAYER_IDLE;
                }
                break;
            }
        }

        // Add acceeleration to velocity
        obj.velocity += currentDirection * obj.acceleration * deltaTime;
        if (std::abs(obj.velocity.x) > obj.maxSpeedX) {
            obj.velocity.x = currentDirection * obj.maxSpeedX;
        }
        // Add velocity to position
        obj.position += obj.velocity * deltaTime;
    }
}

void createTiles(const SDLState& state, GameState& gs, const Resources& res) {
    /*
      1 - Ground
      2 - Panel
      3 - Enemy
      4 - Player
      5 - Grass
      6 - Brick
    */

    short map[MAP_ROWS][MAP_COLS];

    for (int r = 0; r < MAP_ROWS; r++)
        for (int c = 0; c < MAP_COLS; c++)
            map[r][c] = 0;

    for (int c = 0; c < MAP_COLS; c++) {
        map[4][c] = 1;
    }

    map[0][1] = 4;

    const auto createObject = [&state](int r, int c, SDL_Texture* tex, ObjectType type) {
        GameObject o;
        o.type = type;
        o.position = glm::vec2(c * TILE_SIZE, state.logH - (MAP_ROWS - r) * TILE_SIZE);
        o.texture = tex;
        return o;
    };

    for (int r = 0; r < MAP_ROWS; r++) {
        for (int c = 0; c < MAP_COLS; c++) {
            switch (map[r][c]) {
                case 1: {
                    // Ground Tile
                    GameObject ground = createObject(r, c, res.texGround, ObjectType::level);
                    gs.layers[Layer_IDX_LEVEL].push_back(ground);
                    break;
                }
                case 2: {
                    GameObject panel = createObject(r, c, res.texPanel, ObjectType::level);
                    gs.layers[Layer_IDX_LEVEL].push_back(panel);
                    break;
                }
                case 3: {
                    break;
                }
                case 4: {
                    // Create the Player
                    GameObject player = createObject(r, c, res.texIdle, ObjectType::player);
                    player.data.player = PlayerData();
                    player.texture = res.texIdle;
                    player.animations = res.playerAnims;
                    player.currentAnimation = res.ANIM_PLAYER_IDLE;
                    player.acceleration = glm::vec2(300, 0);
                    player.maxSpeedX = 100;
                    gs.layers[Layer_IDX_CHARACTERS].push_back(player);

                    break;
                }
            }
        }
    }
}