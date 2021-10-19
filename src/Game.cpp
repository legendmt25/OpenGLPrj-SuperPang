#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <thread>
#include <chrono>

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <filesystem>
#include <experimental/filesystem>

#include "Game.h"
#include "ResourceManager.h"
#include "SpriteRenderer.h"
#include "Sprite3DRenderer.h"
#include "BallObject.h"
#include "PlayerObject.h"
#include "TextRenderer.h"
#include "irrKlang.h"
#include "Utility.h"

SpriteRenderer* Renderer = nullptr;
Sprite3DRenderer* Renderer3D = nullptr;
PlayerObject* Player = nullptr;
TextRenderer* Text = nullptr;
irrklang::ISoundEngine* SoundEngine = nullptr;
const unsigned int lives = 5;
bool Game::Keys[1024] = { 0 };
bool Game::KeysProcessed[1024] = { 0 };

Game::Game(unsigned int width, unsigned int height) 
    : State(GAME_MENU), Width(width), Height(height), Lives(lives), Level(0) {}

Game::~Game()
{
    for (auto& level : this->Levels) {
        delete level;
    }
    delete Renderer;
    delete Renderer3D;
    delete Player;
    delete Text;
    delete SoundEngine;
}

void Game::LoadFiles() {
    //load shaders
    ResourceManager::LoadShader("../shaders/sprite.vs", "../shaders/sprite.fs", NULL, "sprite");
    ResourceManager::LoadShader("../shaders/text.vs", "../shaders/text.fs", NULL, "text");
    ResourceManager::LoadShader("../shaders/sprite3D.vs", "../shaders/sprite3D.fs", NULL, "sprite3D");
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

    ResourceManager::GetShader("sprite3D").SetInteger("image", 0, true);
    ResourceManager::GetShader("sprite3D").SetMatrix4("projection", glm::ortho(0.0f, (float)this->Width, (float)this->Height, 0.0f, 0.0f, 1.0f));
    ResourceManager::GetShader("sprite3D").SetMatrix4("view", glm::mat4(1.0f));

    //create game menu
    Option option1("GAME START");
    Option option2("SETTINGS");
    option1.AlignCenter(this->Width, this->Height, -20.0f);
    option2.AlignCenter(this->Width, this->Height, 0.0f);
    this->Menu = GameMenu({ option1, option2 });

    //init Player
    glm::vec3 PlayerVelocity(500.0f);
    glm::vec3 PlayerSize(50.0f, 50.0f, 1.0f);
    //glm::vec3 PlayerPosition(0.0f, 0.0f, 0.0f);
    
    //init objects
    Player = new PlayerObject(glm::vec3(0.0f), glm::vec3(PlayerSize.x, PlayerSize.y, PlayerSize.z), ResourceManager::GetTexture("character-init"), glm::vec3(1.0f), PlayerVelocity);
    Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
    Renderer3D = new Sprite3DRenderer(ResourceManager::GetShader("sprite3D"));
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

        GameLevel& currentLevel = *this->Levels[Level];
        if (currentLevel.isCompleted()) {
            if (this->Level == this->Levels.size() - 1) {
                this->State = GAME_WIN;
            }
            else {
                ++this->Level;
            }
            currentLevel.Reset();
        }

        for (auto& Weapon : Player->Weapons) {
            if (Weapon->Using) {
                Weapon->Move(dt, this->Width, this->Height);
            }
            else {
                Weapon->Using = false;
                Weapon->Reset();
            }
        }

        if (this->Lives == 0) {
            this->State = GAME_MENU;
        }
    }
}


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

        Player->ProcessInput(dt, this->Width, this->Height);

    }

    if (this->State == GAME_MENU) {
        if (this->Keys[GLFW_KEY_ENTER] && this->Menu.Selected == 0 && !this->KeysProcessed[GLFW_KEY_ENTER]) {
            this->Reset();
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
        Renderer3D->DrawSprite(ResourceManager::GetTexture("background" + std::to_string(this->Level + 1)), glm::vec3(this->Width / 2.0f, this->Height / 2.0f, 0.0f), glm::vec3(this->Width, this->Height, 1.0f), 0.0f, glm::vec3(1.0f));
        
        for (auto& Weapon : Player->Weapons) {
            if (Weapon->Using) {
                Weapon->Draw(*Renderer3D);
            }
        }
        
        Player->Draw(*Renderer3D);
        this->Levels[this->Level]->Draw(*Renderer3D);
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
    for (auto& object : this->Levels[this->Level]->Objects) {
        if (!object->Destroyed && dynamic_cast<BallObject*>(object) != nullptr) {
            Collision& collisionPlayer = object->checkCollision(*Player);
            if (collisionPlayer.collision) {
                --this->Lives;
                SoundEngine->stopAllSounds();
                std::this_thread::sleep_for(std::chrono::seconds(1));
                this->Levels[Level]->Reset();
                Player->Reset(glm::vec3((this->Width - Player->Size.x) / 2.0f, this->Height - Player->Size.y / 2.0f, 0.0f), glm::vec3(500.0f));
                SoundEngine->play2D("../audio/breakout.mp3", true);
            }

            for (auto& Weapon : Player->Weapons) {
                Collision& collisionWeapon = object->checkCollision(*Weapon);
                if (Weapon->Using && collisionWeapon.collision) {
                    if (dynamic_cast<BallObject*>(object) != nullptr) {
                        object->Destroyed = true;
                    }
                    Weapon->Using = false;
                    Weapon->Reset();
                }
            }
        }
    }
}

void Game::Reset() {
    Player->Reset(glm::vec3((this->Width - Player->Size.x) / 2.0f, this->Height - Player->Size.y / 2.0f, 0.0f), glm::vec3(500.0f, 500.0f, 0.0f));
    this->Level = 0;
    this->Levels[this->Level]->Reset();
    this->Lives = lives;
}