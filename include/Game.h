#ifndef GAME_H
#define GAME_H

#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "ft2build.h"
#include FT_FREETYPE_H

#include <GameLevel.h>
#include <GameMenu.h>

enum GameState {
    GAME_PAUSE,
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

class Game
{
public:
    // game state
    GameMenu Menu;
    GameState State;	
    bool Keys[1024];
    bool KeysProcessed[1024];
    unsigned int Width, Height;
    std::vector<GameLevel*> Levels;
    unsigned int Level;
    unsigned int Lives;

    Game(unsigned int width, unsigned int height);
    ~Game();

    // initialize game state (load all shaders/textures/levels)
    void Init();
    void ProcessInput(float dt);
    void Update(float dt);
    void Render();
    void DoCollisions();
    void Reset();
};

#endif

