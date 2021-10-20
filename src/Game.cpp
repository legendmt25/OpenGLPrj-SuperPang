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
#include "PowerUpObject.h"
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
std::vector<std::string> powerups;
bool PlayerCollision = false;


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
    std::vector<std::string> texturesDirectories = { "../textures/", "../levels/backgrounds/", "../textures/powerups/" };
    //load textures
    for (auto& dir : texturesDirectories) {
        for (auto& file : std::experimental::filesystem::directory_iterator(dir)) {
            if (file.path().extension().string() == "") {
                continue;
            }
            if (file.path().string().find("powerups") != -1) {
                powerups.push_back(file.path().filename().replace_extension().string());
            }
            ResourceManager::LoadTexture(file.path().string().c_str(), file.path().extension() == ".png" && file.path().string().find("levels") == -1 && file.path().filename().string()[0] != '-', file.path().filename().replace_extension().string());
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
            BallObject* Ball = dynamic_cast<BallObject*>(object);
            if (Ball != nullptr && Ball->pop) {
                Ball->Pop(dt);
            }
            object->Move(dt, this->Width, this->Height);
        }

        this->DoCollisions();

        if (!PlayerCollision) {
            Player->Move(dt, this->Width, this->Height);
        }

        GameLevel& currentLevel = *this->Levels[Level];
        if (currentLevel.isCompleted()) {
            if (this->Level == this->Levels.size() - 1) {
                this->State = GAME_WIN;
            }
            else {
                Player->Reset(glm::vec3((this->Width - Player->Size.x) / 2.0f, this->Height - Player->Size.y / 2.0f, 0.0f), glm::vec3(500.0f, 500.0f, 0.0f));
                Player->ResetWeapons();
                ++this->Level;
            }
            currentLevel.Reset();
        }


        for (int i = 0; i < this->Levels[this->Level]->PowerUps.size();) {
            auto& el = this->Levels[this->Level]->PowerUps[i];
            if (el->Destroyed) {
                delete el;
                this->Levels[this->Level]->PowerUps.erase(this->Levels[this->Level]->PowerUps.begin() + i, this->Levels[this->Level]->PowerUps.begin() + i + 1);
            }
            else {
                el->Move(dt, this->Width, this->Height);
                ++i;
            }
        }

        for (auto& Weapon : Player->Weapons) {
            if (Weapon->Using) {
                Weapon->Move(dt, this->Width, this->Height);
            }
            else {
                Weapon->Reset(Player);
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

    //COLLISION WITH POWERUPS AND WEAPONS
    for (auto& object : this->Levels[this->Level]->PowerUps) {
        for (auto& Weapon : Player->Weapons) {
            if (Weapon->Using) {
                Collision WeaponPowerUpCollision = object->checkCollision(*Weapon);
                if (WeaponPowerUpCollision.collision) {
                    object->Destroyed = true;
                    Weapon->Reset(Player);
                    object->Activate(Player);
                    break;
                }
            }
        }
    }

    //COLLISION WITH POWERUPS AND PLAYER
    for (auto& object : this->Levels[this->Level]->PowerUps) {
        Collision PlayerPowerUpCollision = object->checkCollision(*Player);
        if (PlayerPowerUpCollision.collision) {
            object->Destroyed = true;
            object->Activate(Player);
        }
    }

    //COLLISION WITH WEAPONS
    for (int i = 0; i < this->Levels[this->Level]->Objects.size(); ++i) {
        auto& object = this->Levels[this->Level]->Objects[i];
        if (object->Destroyed) {
            continue;
        }
        BallObject* Ball = dynamic_cast<BallObject*>(object);
        for (auto& Weapon : Player->Weapons) {
            if (!Weapon->Using) {
                continue;
            }

            Collision& collisionWeapon = object->checkCollision(*Weapon);
            if (collisionWeapon.collision) {
                if (!object->IsSolid) {
                    Weapon->Using = false;
                    //generatePowerUp
                    unsigned int r = rand() % 100;
                    if (r >= 20 && r <= 30) {
                        unsigned int randomPowerUp = rand() % powerups.size();
                        this->Levels[this->Level]->PowerUps.push_back(new PowerUpObject(*object, ResourceManager::GetTexture(powerups[randomPowerUp]), powerups[randomPowerUp]));
                    }

                    //Create new balls half the size of the collision ball
                    if (Ball != nullptr && object->Size.x / 4.0f > 10.0f) {
                        Ball->pop = true;
                        glm::vec3 Position = object->Position;
                        float radius = object->Size.x / 4.0f;
                        BallObject* a = new BallObject(Position, radius, ResourceManager::GetTexture("ball"), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(130.0f, 190.0f, 0.0f));
                        BallObject* b = new BallObject(Position, radius, ResourceManager::GetTexture("ball"), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(130.0f, 190.0f, 0.0f));
                        a->Velocity = -a->Velocity;
                        b->Velocity.y = -b->Velocity.y;

                        this->Levels[this->Level]->Objects.push_back(a);
                        this->Levels[this->Level]->Objects.push_back(b);
                        break;
                    }
                    else {
                        object->Destroyed = true;
                    }
                }
                
                if (dynamic_cast<PowerArrowObject*>(Weapon) != nullptr) {
                    dynamic_cast<PowerArrowObject*>(Weapon)->Stuck = true;
                }
                else {
                    Weapon->Using = false;
                }
            }
        }
    }



    //COLLISION BALLS WITH OTHER OBJECTS IN LEVEL
    for (auto& object : this->Levels[this->Level]->Objects) {
        BallObject* Ball = dynamic_cast<BallObject*>(object);
        if (object->Destroyed || Ball == nullptr) {
            continue;
        }
        
        for (auto& obj : this->Levels[this->Level]->Objects) {
            BlockObject* Obj = dynamic_cast<BlockObject*>(obj);
            if (obj->Destroyed || Obj == nullptr)
                continue;

            Collision& collisionBallObj = Ball->checkCollision(*Obj);
            if (collisionBallObj.collision) {
                if (collisionBallObj.direction == LEFT || collisionBallObj.direction == RIGHT)
                {
                    Ball->Velocity.x = -Ball->Velocity.x;
                    float penetration = Ball->Radius - std::abs(collisionBallObj.difference.x);
                    if (collisionBallObj.direction == LEFT)
                        Ball->Position.x += penetration;
                    else
                        Ball->Position.x -= penetration;
                }
                else {
                    Ball->Velocity.y = -Ball->Velocity.y;
                    float penetration = Ball->Radius - std::abs(collisionBallObj.difference.y);
                    if (collisionBallObj.direction == UP)
                        Ball->Position.y -= penetration;
                    else
                        Ball->Position.y += penetration;
                }
            }
        }
    }

    //PLAYER BALL COLLISIONS
    for (int i = 0; i < this->Levels[this->Level]->Objects.size(); ++i) {
        auto& object = this->Levels[this->Level]->Objects[i];
        if (object->Destroyed) {
            continue;
        }
        Collision& collisionBallPlayer = object->checkCollision(*Player);
        if (collisionBallPlayer.collision) {
            if (dynamic_cast<BallObject*>(object) != nullptr) {
                --this->Lives;
                SoundEngine->stopAllSounds();
                std::this_thread::sleep_for(std::chrono::seconds(1));
                this->Levels[Level]->Reset();
                Player->Reset(glm::vec3((this->Width - Player->Size.x) / 2.0f, this->Height - Player->Size.y / 2.0f, 0.0f), glm::vec3(500.0f));
                Player->ResetWeapons();
                SoundEngine->play2D("../audio/breakout.mp3", true);
            }
            else if(dynamic_cast<BlockObject*>(object) != nullptr) {
                //TODO: keep the player on the block
                PlayerCollision = true;
            }
        }
        else {
            PlayerCollision = false;
        }
    }


}

void Game::Reset() {
    Player->Reset(glm::vec3((this->Width - Player->Size.x) / 2.0f, this->Height - Player->Size.y / 2.0f, 0.0f), glm::vec3(500.0f, 500.0f, 0.0f));
    //Player->Reset(glm::vec3(480.0f, 0.0f, 0.0f), glm::vec3(500.0f, 500.0f, 0.0f));
    this->Level = 0;
    this->Levels[this->Level]->Reset();
    this->Lives = lives;
}