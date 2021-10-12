#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <thread>

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <filesystem>
#include <experimental/filesystem>

#include "Game.h"
#include "ResourceManager.h"
#include "SpriteRenderer.h"
#include "BallObject.h"
#include "PlayerObject.h"
#include "TextRenderer.h"
#include "irrKlang.h"

SpriteRenderer* Renderer = nullptr;
PlayerObject* Player = nullptr;
TextRenderer* Text = nullptr;
irrklang::ISoundEngine* SoundEngine = nullptr;
const unsigned int lives = 5;

Game::Game(unsigned int width, unsigned int height) 
    : State(GAME_MENU), Keys(), Width(width), Height(height), Lives(lives), Level(0) {}

Game::~Game()
{
    for (auto& level : this->Levels) {
        delete level;
    }
    delete Renderer;
    delete Player;
    delete Text;
    delete SoundEngine;
}

void Game::LoadFiles() {
    //load shaders
    ResourceManager::LoadShader("../shaders/shader.vs", "../shaders/shader.fs", NULL, "sprite");
    ResourceManager::LoadShader("../shaders/text.vs", "../shaders/text.fs", NULL, "text");
    std::vector<std::string> texturesDirectories = { "../textures/", "../levels/backgrounds/" };
    //load textures
    for (auto& dir : texturesDirectories) {
        for (auto& file : std::experimental::filesystem::directory_iterator(dir)) {
            ResourceManager::LoadTexture(file.path().string().c_str(), file.path().extension() == ".png" && file.path().string().find("levels") == -1 , file.path().filename().replace_extension().string());
        }
    }

    //load levels
    for (auto& file : std::experimental::filesystem::directory_iterator("../levels/")) {
        GameLevel* level = new GameLevel(file.path().string().c_str());
        level->Load(this->Width, this->Height);
        this->Levels.push_back(level);
    }

    std::ifstream gameMenuFile("game.menu");

}


void Game::Init()
{
    this->LoadFiles();
    ResourceManager::GetShader("sprite").SetInteger("image", 0, true);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", glm::ortho(0.0f, (float)this->Width, (float)this->Height, 0.0f, -1.0f, 1.0f));

    //create game menu
    Option option1("GAME START");
    Option option2("SETTINGS");
    option1.AlignCenter(this->Width, this->Height, -20.0f);
    option2.AlignCenter(this->Width, this->Height, 0.0f);
    this->Menu = GameMenu({ option1, option2 });

    //init Player
    glm::vec2 PlayerVelocity(500.0f);
    glm::vec2 PlayerSize(50.0f, 50.0f);
    glm::vec2 PlayerPosition((this->Width - PlayerSize.x) / 2.0f, this->Height - PlayerSize.y);
    
    //init objects
    Player = new PlayerObject(PlayerPosition, PlayerSize, ResourceManager::GetTexture("character-init"), glm::vec3(1.0f), PlayerVelocity);
    Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
    Text = new TextRenderer(this->Width, this->Height);
    SoundEngine = irrklang::createIrrKlangDevice();

    Text->Load("../fonts/OCRAEXT.TTF", 24);
    SoundEngine->play2D("../audio/breakout.mp3", true);
}

void Game::Update(float dt)
{
    if (this->State == GAME_ACTIVE) {
        for (auto& object : this->Levels[Level]->Objects) {
            object->Move(dt, this->Width, this->Height);
        }
        this->DoCollisions();

        GameLevel& currentLevel = *(this->Levels[Level]);
        if (currentLevel.isCompleted()) {
            currentLevel.Reset();
            this->State = GAME_WIN;
        }
        Player->Weapon->Move(dt, this->Width, this->Height);
    }
}

unsigned int PlayerTexture = 0;
float frames = 0;

void Game::ProcessInput(float dt)
{
    if (this->State == GAME_PAUSE) {
        if (this->Keys[GLFW_KEY_ENTER] && !this->KeysProcessed[GLFW_KEY_ENTER]) {
            this->State = GAME_ACTIVE;
            this->KeysProcessed[GLFW_KEY_ENTER] = true;
        }
    }

    if (this->State == GAME_ACTIVE) {
        if (this->Keys[GLFW_KEY_ENTER] && !this->KeysProcessed[GLFW_KEY_ENTER]) {
            this->State = GAME_PAUSE;
            this->KeysProcessed[GLFW_KEY_ENTER] = true;
        }

        std::string direction = "";

        if (this->Keys[GLFW_KEY_LEFT]) {
            if (Player->Position.x >= 0) {
                Player->Position.x -= Player->Velocity.x * dt;
            }
            direction = "-left-";
        }

        if (this->Keys[GLFW_KEY_RIGHT]) {
            if (Player->Position.x + Player->Size.x <= this->Width) {
                Player->Position.x += Player->Velocity.x * dt;
            }
            direction = "-right-";
        }
        
        if (!this->Keys[GLFW_KEY_RIGHT] && !this->Keys[GLFW_KEY_LEFT]) {
            Player->Texture = ResourceManager::GetTexture("character-init");
            PlayerTexture = 1;
        }
        else {
            if (frames >= 0.1f) {
                frames = 0;
                PlayerTexture = (PlayerTexture) % 4 + 1;
            }
            else {
                frames += dt;
            }
            Player->Texture = ResourceManager::GetTexture("character-walk" + direction + std::to_string(PlayerTexture));
        }

        if (this->Keys[GLFW_KEY_X] || this->Keys[GLFW_KEY_SPACE]) {
            Player->Texture = ResourceManager::GetTexture("character-shoot");
            if (!Player->Weapon->Using) {
                Player->Shoot();
            }
        }
    }

    if (this->State == GAME_MENU) {
        if (this->Keys[GLFW_KEY_ENTER] && this->Menu.Selected == 0 && !this->KeysProcessed[GLFW_KEY_ENTER]) {
            this->State = GAME_ACTIVE;
            this->KeysProcessed[GLFW_KEY_ENTER] = true;
        }
        if (this->Keys[GLFW_KEY_S] && !this->KeysProcessed[GLFW_KEY_S]) {
            this->KeysProcessed[GLFW_KEY_S] = true;
            if (this->Menu.Selected < this->Menu.Options.size() - 1) {
                this->Menu.Options[this->Menu.Selected].Selected = false;
                ++this->Menu.Selected;
                this->Menu.Options[this->Menu.Selected].Selected = true;
            }
        }
        if (this->Keys[GLFW_KEY_W] && !this->KeysProcessed[GLFW_KEY_W]) {
            this->KeysProcessed[GLFW_KEY_W] = true;
            if (this->Menu.Selected > 0) {
                this->Menu.Options[this->Menu.Selected].Selected = false;
                --this->Menu.Selected;
                this->Menu.Options[this->Menu.Selected].Selected = true;
            }
        }
    }

    if (this->State == GAME_WIN)
    {
        if (this->Keys[GLFW_KEY_ENTER])
        {
            this->KeysProcessed[GLFW_KEY_ENTER] = true;
            this->State = GAME_MENU;
        }
    }
}

void Game::Render()
{

    if (this->State == GAME_ACTIVE || this->State == GAME_PAUSE) {
        Renderer->DrawSprite(ResourceManager::GetTexture("background" + std::to_string(this->Level + 1)), glm::vec2(0.0f), glm::vec2(this->Width, this->Height), 0.0f, glm::vec3(1.0f));
        if (Player->Weapon->Using) {
            Player->Weapon->Draw(*Renderer);
        }
        Player->Draw(*Renderer);
        this->Levels[this->Level]->Draw(*Renderer);
        Text->RenderText("Lives:" + std::to_string(this->Lives), 5.0f, this->Height - 20.0f, 1.0f);
        

        if (this->State == GAME_PAUSE) {
            Text->RenderText("PAUSED", (this->Width - 72.0f * 5.0f) / 2.0f, this->Height / 2.0f - 20.0f, 5.0f);
        }
    }

    if (this->State == GAME_MENU) {
        Renderer->DrawSprite(ResourceManager::GetTexture("background-menu"), glm::vec2(0.0f), glm::vec2(this->Width, this->Height), 0.0f, glm::vec3(1.0f));
        Menu.Draw(*Text);
    }

    if (this->State == GAME_WIN)
    {
        Text->RenderText("You WON!!!", 320.0, Height / 2 - 20.0, 1.0, glm::vec3(0.0, 1.0, 0.0));
        Text->RenderText("Press ENTER to retry or ESC to quit", 130.0, Height / 2, 1.0, glm::vec3(1.0, 1.0, 0.0));
    }
}

void Game::DoCollisions() {

}

void Game::Reset() {
    this->Level = 0;
    this->Levels[this->Level]->Reset();
    this->State = GAME_MENU;
    this->Lives = lives;
}