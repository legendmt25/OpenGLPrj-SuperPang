#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <thread>

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
    : State(GAME_MENU), Keys(), Width(width), Height(height), Lives(lives) {}

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

void Game::Init()
{
    ResourceManager::LoadShader("OpenGLPrj/shaders/shader.vs", "OpenGLPrj/shaders/shader.fs", NULL, "sprite");
    ResourceManager::LoadTexture("OpenGLPrj/textures/awesomeface.png", true, "face");
    ResourceManager::LoadTexture("OpenGLPrj/textures/background.jpg", false, "background_menu");
    ResourceManager::LoadTexture("OpenGLPrj/textures/block.png", false, "block");
    ResourceManager::LoadTexture("OpenGLPrj/textures/block_solid.png", false, "block_solid");
    ResourceManager::LoadTexture("OpenGLPrj/textures/paddle.png", true, "paddle");
    ResourceManager::LoadTexture("OpenGLPrj/textures/ball.png", true, "crystal_ball");
    ResourceManager::LoadTexture("OpenGLPrj/textures/character-init.png", true, "character-init");
    ResourceManager::LoadTexture("OpenGLPrj/textures/character-walk-left-1.png", true, "character-walk-left-0");
    ResourceManager::LoadTexture("OpenGLPrj/textures/character-walk-left-2.png", true, "character-walk-left-1");
    ResourceManager::LoadTexture("OpenGLPrj/textures/character-walk-left-3.png", true, "character-walk-left-2");
    ResourceManager::LoadTexture("OpenGLPrj/textures/character-walk-left-4.png", true, "character-walk-left-3");
    ResourceManager::LoadTexture("OpenGLPrj/textures/character-walk-right-1.png", true, "character-walk-right-0");
    ResourceManager::LoadTexture("OpenGLPrj/textures/character-walk-right-2.png", true, "character-walk-right-1");
    ResourceManager::LoadTexture("OpenGLPrj/textures/character-walk-right-3.png", true, "character-walk-right-2");
    ResourceManager::LoadTexture("OpenGLPrj/textures/character-walk-right-4.png", true, "character-walk-right-3");
    ResourceManager::LoadTexture("OpenGLPrj/textures/character-shoot.png", true, "character-shoot");
    ResourceManager::LoadTexture("OpenGLPrj/textures/arrow.png", true, "arrow");


    GameLevel* one = new GameLevel("OpenGLPrj/levels/1.level");
    one->Load(this->Width, this->Height);
    GameLevel* two = new GameLevel("OpenGLPrj/levels/2.level");
    two->Load(this->Width, this->Height);

    std::string Value = "GAME START";
    float fontSize = 12;
    Option option1(Value, glm::vec2((this->Width - Value.size() * fontSize) / 2.0f, this->Height / 2.0f - 20.0f));

    Value = "SETTINGS";
    Option option2(Value, glm::vec2((this->Width - Value.size() * fontSize) / 2.0f, this->Height / 2.0f));
    
    Menu = GameMenu({ option1, option2 });

    this->Levels = { one, two };
    this->Level = 0;

    for (int i = 1; i <= this->Levels.size(); ++i) {
        char str[100] = "OpenGLPrj/levels/backgrounds/";
        strcat(str, std::to_string(i).c_str());
        strcat(str, ".png");

        char ch[50] = "background";
        strcat(ch, std::to_string(i).c_str());
        ResourceManager::LoadTexture(str, false, ch);
    }

    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);

    glm::mat4 projection = glm::ortho(0.0f, (float)this->Width, (float)this->Height, 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);

    Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
    Text = new TextRenderer(this->Width, this->Height);
    SoundEngine = irrklang::createIrrKlangDevice();

    glm::vec2 PlayerVelocity(500.0f);
    glm::vec2 PlayerSize(50.0f, 50.0f);
    glm::vec2 PlayerPosition((this->Width - PlayerSize.x) / 2.0f, this->Height - PlayerSize.y);
    Player = new PlayerObject(PlayerPosition, PlayerSize, ResourceManager::GetTexture("character-init"), glm::vec3(1.0f), PlayerVelocity);

    Text->Load("OpenGLPrj/fonts/OCRAEXT.TTF", 24);
    SoundEngine->play2D("OpenGLPrj/audio/breakout.mp3", true);
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

        if (Player->Weapon != nullptr) {
            if (Player->Weapon->Position.y > 0) {
                Player->Weapon->Position.y -= Player->Weapon->Velocity.y * dt;
                Player->Weapon->Size.y += Player->Weapon->Velocity.y * dt;
            }
            else {
                delete Player->Weapon;
                Player->Weapon = nullptr;
            }
        }
    }
}

unsigned int PlayerTexture = 0;
float oldPositionX = 0;

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
            PlayerTexture = 0;
        }
        else {
            if (abs(Player->Position.x - oldPositionX) > 100.0f) {
                oldPositionX = Player->Position.x;
                PlayerTexture = (PlayerTexture + 1) % 4;
            }
            Player->Texture = ResourceManager::GetTexture("character-walk" + direction + std::to_string(PlayerTexture));
        }

        if (this->Keys[GLFW_KEY_X] || this->Keys[GLFW_KEY_SPACE]) {
            Player->Texture = ResourceManager::GetTexture("character-shoot");
            if (Player->Weapon == nullptr) {
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
        char ch[50] = "background";
        strcat(ch, std::to_string(this->Level + 1).c_str());
        
        Renderer->DrawSprite(ResourceManager::GetTexture(ch), glm::vec2(0.0f), glm::vec2(this->Width, this->Height), 0.0f, glm::vec3(1.0f));
        Player->Draw(*Renderer);
        this->Levels[this->Level]->Draw(*Renderer);
        Text->RenderText("Lives:" + std::to_string(this->Lives), 5.0f, this->Height - 20.0f, 1.0f);
        if (Player->Weapon != nullptr) {
            Player->Weapon->Draw(*Renderer);
        }

        if (this->State == GAME_PAUSE) {
            Text->RenderText("PAUSED", (this->Width - 72.0f * 5.0f) / 2.0f, this->Height / 2.0f - 20.0f, 5.0f);
        }
    }

    if (this->State == GAME_MENU) {
        Renderer->DrawSprite(ResourceManager::GetTexture("background_menu"), glm::vec2(0.0f), glm::vec2(this->Width, this->Height), 0.0f, glm::vec3(1.0f));
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