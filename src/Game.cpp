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
#include "LadderObject.h"
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
bool PlayerLadderCollision = false;
bool PlayerBlockCollision = false;

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
            //std::cout << file.path().string() << std::endl;
            ResourceManager::LoadTexture(file.path().string().c_str(), file.path().extension() == ".png" && file.path().string().find("levels") == -1 && file.path().filename().string()[0] != '-', file.path().filename().replace_extension().string());
        }
    }

    //load levels
    for (auto& file : std::experimental::filesystem::directory_iterator("../levels/")) {
        if (file.path().string().find("background") != -1) {
            continue;
        }
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
    SoundEngine->play2D("../audio/mode-select.mp3", true);
}

void Game::Update(float dt)
{
    if (this->State == GAME_ACTIVE) {
        for (auto& object : this->Levels[Level]->Objects) {
            object->Move(dt, this->Width, this->Height);
        }
        
        for (auto& object : this->Levels[Level]->Attackers) {
            if (object->pop) {
                object->Pop(dt);
            }
            object->Move(dt, this->Width, this->Height);
        }

        this->DoCollisions();

        if (!PlayerBlockCollision && !PlayerLadderCollision) {
            Player->Move(dt, this->Width, this->Height);
        }

        GameLevel& currentLevel = *this->Levels[Level];
        if (currentLevel.isCompleted()) {
            if (this->Level == this->Levels.size() - 1) {
                this->State = GAME_WIN;
                SoundEngine->stopAllSounds();
                SoundEngine->play2D("../audio/congratulations.mp3");
                std::this_thread::sleep_for(std::chrono::seconds(6));
            }
            else {
                SoundEngine->stopAllSounds();
                SoundEngine->play2D("../audio/stage-clear.mp3");
                std::this_thread::sleep_for(std::chrono::seconds(4));
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

        Player->ProcessInput(dt, this->Width, this->Height, PlayerLadderCollision);

    }

    if (this->State == GAME_MENU) {
        if (this->Keys[GLFW_KEY_ENTER] && this->Menu.Selected == 0 && !this->KeysProcessed[GLFW_KEY_ENTER]) {
            SoundEngine->stopAllSounds();
            SoundEngine->play2D("../audio/stage1.mp3", true);
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
        
        this->Levels[this->Level]->Draw(*Renderer3D);
        
        for (auto& Weapon : Player->Weapons) {
            if (Weapon->Using) {
                Weapon->Draw(*Renderer3D);
            }
        }
        Player->Draw(*Renderer3D);

        for (auto& object : this->Levels[this->Level]->Attackers) {
            if (!object->Destroyed) {
                object->Draw(*Renderer3D);
            }
        }

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

    //COLLISION POWERUPS AND WEAPONS
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

    //COLLISION POWERUPS AND PLAYER
    for (auto& object : this->Levels[this->Level]->PowerUps) {
        Collision PlayerPowerUpCollision = object->checkCollision(*Player);
        if (PlayerPowerUpCollision.collision) {
            object->Destroyed = true;
            object->Activate(Player);
        }
    }

    //COLLISION ATTACKERS AND WEAPONS
    for (int i = 0; i < this->Levels[this->Level]->Attackers.size(); ++i) {
        auto& object = this->Levels[this->Level]->Attackers[i];
        if (object->Destroyed || object->pop) {
            continue;
        }

        for (auto& Weapon : Player->Weapons) {
            if (!Weapon->Using) {
                continue;
            }

            Collision& collisionWeapon = object->checkCollision(*Weapon);
            if (collisionWeapon.collision && !object->IsSolid) {
                object->pop = true;
                Weapon->Using = false;
                //generatePowerUp
                unsigned int r = rand() % 100;
                if (r >= 20 && r <= 30) {
                    unsigned int randomPowerUp = rand() % powerups.size();
                    this->Levels[this->Level]->PowerUps.push_back(new PowerUpObject(*object, ResourceManager::GetTexture(powerups[randomPowerUp]), powerups[randomPowerUp]));
                }

                //Create new balls half the size of the collision ball

                SoundEngine->play2D("../audio/ball-pop.mp3");
                if (object && object->Size.x / 4.0f > 12.0f) {
                    object->pop = true;
                    glm::vec3 Position = object->Position;
                    float radius = object->Size.x / 4.0f;

                    AttackerObject* a = nullptr;
                    AttackerObject* b = nullptr;
                    if(dynamic_cast<HexagonObject*>(object)) {
                        a = new HexagonObject(Position, glm::vec3(radius * 2.0f, radius * 2.0f, 1.0f), ResourceManager::GetTexture("hexagon-1"), object->Color, glm::vec3(130.0f, 190.0f, 0.0f));
                        b = new HexagonObject(Position, glm::vec3(radius * 2.0f, radius * 2.0f, 1.0f), ResourceManager::GetTexture("hexagon-1"), object->Color, glm::vec3(130.0f, 190.0f, 0.0f));
                        a->Velocity = -a->Velocity;
                        b->Velocity.y = -b->Velocity.y;
                    }
                    else if(dynamic_cast<BallObject*>(object)) {
                        a = new BallObject(Position, radius, ResourceManager::GetTexture("ball"), object->Color, glm::vec3(130.0f, 190.0f, 0.0f));
                        b = new BallObject(Position, radius, ResourceManager::GetTexture("ball"), object->Color, glm::vec3(130.0f, 190.0f, 0.0f));
                        a->Velocity = -a->Velocity;
                        b->Velocity.y = -b->Velocity.y;
                    }

                    this->Levels[this->Level]->Attackers.push_back(a);
                    this->Levels[this->Level]->Attackers.push_back(b);
                    break;
                }
            }
        }
    }

    //COLLISION OBJECTS AND WEAPONS
    for (auto& object : this->Levels[this->Level]->Objects) {
        if (object->Destroyed || dynamic_cast<LadderObject*>(object)) {
            continue;
        }

        for (auto& Weapon : Player->Weapons) {
            if (!Weapon->Using) {
                continue;
            }
            Collision collisionObjectWeapon = object->checkCollision(*Weapon);
            if (collisionObjectWeapon.collision) {
                if (object->IsSolid) {
                    if (dynamic_cast<PowerArrowObject*>(Weapon)) {
                        dynamic_cast<PowerArrowObject*>(Weapon)->Stuck = true;
                    }
                    else {
                        Weapon->Reset(Player);
                    }
                }
                else {
                    object->Destroyed = true;
                    Weapon->Reset(Player);
                }
            }
        }
    }

    //COLLISION ATTACKERS AND OBJECTS
    for (auto& object : this->Levels[this->Level]->Attackers) {
        if (object->Destroyed || object->pop) {
            continue;
        }
        for (auto& obj : this->Levels[this->Level]->Objects) {
            if (obj->Destroyed)
                continue;

            Collision& collisionBallObj = object->checkCollision(*obj);
            if (collisionBallObj.collision) {
                if (collisionBallObj.direction == LEFT || collisionBallObj.direction == RIGHT)
                {
                    object->Velocity.x = -object->Velocity.x;
                    float penetration = object->Radius - std::abs(collisionBallObj.difference.x);
                    if (collisionBallObj.direction == LEFT)
                        object->Position.x += penetration;
                    else
                        object->Position.x -= penetration;
                }
                else {
                    object->Velocity.y = -object->Velocity.y;
                    float penetration = object->Radius - std::abs(collisionBallObj.difference.y);
                    if (collisionBallObj.direction == UP)
                        object->Position.y -= penetration;
                    else
                        object->Position.y += penetration;
                }
            }
        }
    }

    PlayerBlockCollision = false;
    PlayerLadderCollision = false;
    //COLLISION ATTACKERS AND PLAYER
    for (auto& object : this->Levels[this->Level]->Attackers) {
        if (object->Destroyed || object->pop) {
            continue;
        }

        if (Player->PlayerAttackerCollision(*object)) {
            --this->Lives;
            SoundEngine->stopAllSounds();
            std::this_thread::sleep_for(std::chrono::seconds(1));
            this->Levels[Level]->Reset();
            Player->Reset(glm::vec3((this->Width - Player->Size.x) / 2.0f, this->Height - Player->Size.y / 2.0f, 0.0f), glm::vec3(500.0f));
            Player->ResetWeapons();
            SoundEngine->play2D("../audio/stage1.mp3", true);
        }
    }

    //COLLISION OBJECTS AND PLAYER
    for(auto& object: this->Levels[this->Level]->Objects) {
        if (object->Destroyed) {
            continue;
        }
        Collision& collisionObjectPlayer = object->checkCollision(*Player);
        if (collisionObjectPlayer.collision) {
            if (dynamic_cast<BlockObject*>(object)) {
                PlayerBlockCollision = true;
            }
            else if (dynamic_cast<LadderObject*>(object)) {
                PlayerLadderCollision |= collisionObjectPlayer.collision;
            }
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