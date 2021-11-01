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
    GAME_WIN,
    GAME_OVER,
    GAME_SLEEP
};

class Game
{
public:
    GameMenu* Menu;
    GameState State;	
    static bool Keys[1024];
    static bool KeysProcessed[1024];
    unsigned int Width, Height;
    std::vector<GameLevel*> Levels;
    unsigned int Level;

    Game(GLFWwindow* currentWindow, unsigned int width, unsigned int height);
    ~Game();

    // initialize game state (load all shaders/textures/levels)

    void Init();
    void LoadFiles();
    void ProcessInput(float dt);
    void Update(float dt);
    void Render();
    void DoCollisions();
    void Reset();
    void ResetPlayers(bool withPlayerLives = false);
private:
    GLFWwindow* currentWindow;
    void ShouldGeneratePowerUp(GameObject& object);
};

#endif

