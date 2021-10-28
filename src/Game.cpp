#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <Windows.h>

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
SphereRenderer* RendererSphere_36sectors_18stacks = nullptr;
SphereRenderer* RendererSphere_12sectors_6stacks = nullptr;

PlayerObject* Player;
TextRenderer* Text = nullptr;
irrklang::ISoundEngine* SoundEngine = nullptr;
const unsigned int lives = 5;
bool Game::Keys[1024] = { 0 };
bool Game::KeysProcessed[1024] = { 0 };
std::vector<std::string> powerups;
bool PlayerLadderCollision = false;
bool PlayerBlockCollision = false;


Game::Game(GLFWwindow* currentWindow, unsigned int width, unsigned int height) 
    : State(GameState::GAME_MENU), Width(width), Height(height), Level(0), Menu(new GameMenu("../resources/game.menu")), currentWindow(currentWindow) {}

Game::~Game()
{
    for (auto& level : this->Levels) {
        delete level;
    }
    delete Renderer;
    delete Renderer3D;
    delete Text;
    delete SoundEngine;
    delete Menu;
    delete Player;
}

void Game::LoadFiles() {
    //load shaders
    ResourceManager::LoadShader("../resources/shaders/sprite.vs", "../resources/shaders/sprite.fs", NULL, "sprite");
    ResourceManager::LoadShader("../resources/shaders/text.vs", "../resources/shaders/text.fs", NULL, "text");
    ResourceManager::LoadShader("../resources/shaders/sprite3D.vs", "../resources/shaders/sprite3D.fs", NULL, "sprite3D");
    std::vector<std::string> texturesDirectories = { "../resources/textures/", "../resources/levels/backgrounds/", "../resources/textures/powerups/", "../resources/background-menu" };
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
            bool alpha = file.path().extension() == ".png" && file.path().string().find("levels") == -1 && file.path().filename().string()[0] != '-';
            ResourceManager::LoadTexture(file.path().string().c_str(), alpha, file.path().filename().replace_extension().string());
        }
    }



    //load levels
    for (auto& file : std::experimental::filesystem::directory_iterator("../resources/levels/")) {
        if (file.path().extension().string() == "") {
            continue;
        }
        GameLevel* level = new GameLevel(file.path().string().c_str());
        level->Load(this->Width, this->Height);
        this->Levels.push_back(level);
    }
    this->Menu->Load(this->Width, this->Height);
}



void Game::Init()
{
    std::vector<std::string> frameCounters = { "background", "sleep", "pop-ball", "pop-hexagon", "hexagon", "character-walk", "character-up" , "power-up-destroy"};
    for (auto& frameCounter : frameCounters) {
        frameMap[frameCounter] = 0.0f;
    }

    this->LoadFiles();

    ResourceManager::GetShader("sprite").SetInteger("image", 0, true);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", glm::ortho(0.0f, (float)this->Width, (float)this->Height, 0.0f, -1.0f, 1.0f));

    ResourceManager::GetShader("sprite3D").SetInteger("image", 0, true);
    ResourceManager::GetShader("sprite3D").SetMatrix4("projection", glm::ortho(0.0f, (float)this->Width, (float)this->Height, 0.0f, 0.0f, 100.0f));
    ResourceManager::GetShader("sprite3D").SetMatrix4("view", glm::mat4(1.0f));

    //init Player
    glm::vec3 PlayerVelocity(500.0f);
    glm::vec3 PlayerSize(50.0f, 50.0f, 1.0f);
    //glm::vec3 PlayerPosition(0.0f, 0.0f, 0.0f);
    
    //init objects
    Player = new PlayerObject(glm::vec3(0.0f), glm::vec3(PlayerSize.x, PlayerSize.y, PlayerSize.z), ResourceManager::GetTexture("character-init"), glm::vec3(1.0f), PlayerVelocity, 1);
    Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
    Renderer3D = new Sprite3DRenderer(ResourceManager::GetShader("sprite3D"));
    RendererSphere_36sectors_18stacks = new SphereRenderer(ResourceManager::GetShader("sprite3D"), 18, 36);
    RendererSphere_12sectors_6stacks = new SphereRenderer(ResourceManager::GetShader("sprite3D"), 6, 12);

    Text = new TextRenderer(this->Width, this->Height);
    SoundEngine = irrklang::createIrrKlangDevice();

    Text->Load("../resources/fonts/OCRAEXT.TTF", 24);
    SoundEngine->play2D("../resources/audio/mode-select.mp3", true);
}


int start = 0;
bool wasSleeping = false;
float sleepForFrames;

void Game::Update(float dt)
{
    if (this->State == GAME_SLEEP) {
        if (frameCount(dt, frameMap["background"], sleepForFrames)) {
            this->State = GAME_ACTIVE;
            wasSleeping = true;
        }
    }

    if (this->State == GAME_MENU) {
        if (start < 310 && frameCount(dt, frameMap["sleep"], 0.065f)) {
            ++start;
        }
    }

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

        if (!PlayerBlockCollision && !PlayerLadderCollision) {
            Player->Move(dt, this->Width, this->Height);
        }
        if (this->Levels[Level]->isCompleted()) {
            if (!wasSleeping) {
                SoundEngine->stopAllSounds();
                if (this->Level == this->Levels.size() - 1) {
                    this->Levels[this->Level]->Reset();
                    this->State = GAME_WIN;
                    SoundEngine->play2D("../resources/audio/congratulations.mp3");
                }
                else {
                    this->State = GAME_SLEEP;
                    sleepForFrames = 5.0f;
                    SoundEngine->play2D("../resources/audio/stage-clear.mp3");
                }
            }
            else {
                Player->Reset(glm::vec3((this->Width - Player->Size.x) / 2.0f, this->Height - Player->Size.y / 2.0f, 0.0f), glm::vec3(500.0f, 500.0f, 0.0f));
                Player->ResetWeapons();
                this->Levels[this->Level]->Reset();
                ++this->Level;
                SoundEngine->play2D("../resources/audio/stage1.mp3");
                wasSleeping = false;
            }
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

        if (Player->Lives == 0) {
            this->State = GAME_OVER;
            SoundEngine->play2D("../resources/audio/game-over.mp3");
            start = 0;
        }

        this->DoCollisions();
    }
}

GameState savedGameState;

void Game::ProcessInput(float dt)
{
    if (this->State == GAME_PAUSE) {
        if (this->Keys[GLFW_KEY_ENTER] && !this->KeysProcessed[GLFW_KEY_ENTER]) {
            this->State = savedGameState;
            SoundEngine->setAllSoundsPaused(false);
            this->KeysProcessed[GLFW_KEY_ENTER] = true;
        }
    }

    if (this->State == GAME_ACTIVE || this->State == GAME_SLEEP) {
        if (this->Keys[GLFW_KEY_ENTER] && !this->KeysProcessed[GLFW_KEY_ENTER]) {
            savedGameState = this->State;
            SoundEngine->setAllSoundsPaused();
            this->State = GAME_PAUSE;
            this->KeysProcessed[GLFW_KEY_ENTER] = true;
        }
    }

    if (this->State == GAME_ACTIVE) {
        Player->ProcessInput(dt, this->Width, this->Height, PlayerLadderCollision);
    }

    if (this->State == GAME_MENU) {
       
        if (this->Keys[GLFW_KEY_BACKSPACE] && !this->KeysProcessed[GLFW_KEY_BACKSPACE]) {
            this->KeysProcessed[GLFW_KEY_BACKSPACE] = true;
            if (this->Menu->Selected->ParentOption) {
                this->Menu->Selected = this->Menu->Selected->ParentOption;
                SoundEngine->play2D("../resources/audio/menu-select.mp3");
            }
        }

        if (this->Keys[GLFW_KEY_ENTER] && !this->KeysProcessed[GLFW_KEY_ENTER]) {
            this->KeysProcessed[GLFW_KEY_ENTER] = true;
            if (this->Menu->Selected->Options[this->Menu->Selected->Selected]->Options.size() > 0) {
                this->Menu->Selected = this->Menu->Selected->Options[this->Menu->Selected->Selected];
            }

            auto& val = this->Menu->Selected->Options[this->Menu->Selected->Selected]->Value;
            if (val == "GAME START") {
                SoundEngine->stopAllSounds();
                SoundEngine->play2D("../resources/audio/stage1.mp3", true);
                this->State = GAME_ACTIVE;
                this->Reset();
            }
            if (val == "EXIT") {
                glfwSetWindowShouldClose(currentWindow, true);
            }
            SoundEngine->play2D("../resources/audio/menu-select.mp3");
        }

        if (this->Keys[GLFW_KEY_S] && !this->KeysProcessed[GLFW_KEY_S]) {
            this->KeysProcessed[GLFW_KEY_S] = true;
            if (this->Menu->Selected->Selected < this->Menu->Selected->Options.size() - 1) {
                ++this->Menu->Selected->Selected;
                SoundEngine->play2D("../resources/audio/menu-select.mp3");
            }
        }
        if (this->Keys[GLFW_KEY_W] && !this->KeysProcessed[GLFW_KEY_W]) {
            this->KeysProcessed[GLFW_KEY_W] = true;
            if (this->Menu->Selected->Selected > 0) {
                --this->Menu->Selected->Selected;
                SoundEngine->play2D("../resources/audio/menu-select.mp3");
            }
        }
    }

    if (this->State == GAME_WIN || this->State == GAME_OVER)
    {
        if (this->Keys[GLFW_KEY_ENTER])
        {
            this->KeysProcessed[GLFW_KEY_ENTER] = true;
            this->State = GAME_MENU;
            SoundEngine->stopAllSounds();
            SoundEngine->play2D("../resources/audio/mode-select.mp3");
        }
    }
}



void Game::Render()
{
    if (this->State == GAME_ACTIVE || this->State == GAME_PAUSE || this->State == GAME_SLEEP) {
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
                if (dynamic_cast<BallObject*>(object)) {
                    object->Draw(*RendererSphere_36sectors_18stacks);
                }
                if (dynamic_cast<HexagonObject*>(object)) {
                    object->Draw(*RendererSphere_12sectors_6stacks);
                }
            }
        }


       Text->RenderText("Lives:" + std::to_string(Player->Lives), 5.0f, this->Height - 20.0f, 1.0f);
        

        if (this->State == GAME_PAUSE) {
            Text->RenderText("PAUSED", (this->Width - 72.0f * 5.0f) / 2.0f, this->Height / 2.0f - 20.0f, 5.0f);
        }
    }

    if (this->State == GAME_MENU) {
        std::string st = std::to_string(start);
        std::reverse(st.begin(), st.end());
        while (st.size() < 4) {
            st.push_back('0');
        }
        std::reverse(st.begin(), st.end());

        Renderer->DrawSprite(ResourceManager::GetTexture("background-menu-" + st), glm::vec2(0.0f), glm::vec2(this->Width, this->Height), 0.0f, glm::vec3(1.0f));
        this->Menu->Draw(*Text);
    }

    if (this->State == GAME_WIN || this->State == GAME_OVER) {
        Renderer->DrawSprite(ResourceManager::GetTexture("background"), glm::vec2(0.0f), glm::vec2(this->Width, this->Height), 0.0f, glm::vec3(1.0f));

        std::string st = "Press ENTER to retry or ESC to quit";
        float fontSize = 12.0f;

        Text->RenderText(st, (this->Width - st.size() * fontSize) / 2.0f, this->Height / 2, 1.0, glm::vec3(0.0, 1.0, 0.0));
        if (this->State == GAME_WIN) {
            st = "You WON!!!";
            Text->RenderText(st, (this->Width - st.size() * fontSize) / 2.0f, this->Height / 2 - 20.0, 1.0, glm::vec3(0.0, 1.0, 0.0));
        }
        if (this->State == GAME_OVER) {
            float fontSize = 12.0f;
            st = "GAME OVER!!!";
            Text->RenderText(st, (this->Width - st.size() * fontSize) / 2.0f, this->Height / 2 - 20.0, 1.0, glm::vec3(0.0, 1.0, 0.0));
        }
    }
    
}

void Game::DoCollisions() {

    //COLLISION POWERUPS AND WEAPONS
    for (auto& object : this->Levels[this->Level]->PowerUps) {
        for (auto& Weapon : Player->Weapons) {
            if (!Weapon->Using) {
                continue;
            }
            if (object->checkCollision(*Weapon).collision) {
                object->Destroyed = true;
                Weapon->Reset(Player);
                object->Activate(Player);
                break;
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

            if (object->checkCollision(*Weapon).collision && !object->IsSolid) {
                SoundEngine->play2D("../resources/audio/ball-pop.mp3");
                object->pop = true;
                Weapon->Using = false;

                this->ShouldGeneratePowerUp(*object);

                //Create new balls half the size of the collision ball
                if (object && object->Size.x / 4.0f > 12.0f) {
                    object->pop = true;
                    glm::vec3 Position = object->Position;
                    float radius = object->Size.x / 4.0f;

                    AttackerObject* a = nullptr;
                    AttackerObject* b = nullptr;
                    if (dynamic_cast<HexagonObject*>(object)) {
                        a = new HexagonObject(Position, glm::vec3(radius * 2.0f, radius * 2.0f, 1.0f), ResourceManager::GetTexture("hexagon-1"), object->Color, glm::vec3(130.0f, 190.0f, 0.0f));
                        b = new HexagonObject(Position, glm::vec3(radius * 2.0f, radius * 2.0f, 1.0f), ResourceManager::GetTexture("hexagon-1"), object->Color, glm::vec3(130.0f, 190.0f, 0.0f));
                        a->Velocity = -a->Velocity;
                        b->Velocity.y = -b->Velocity.y;
                    }
                    else if (dynamic_cast<BallObject*>(object)) {
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
            if (object->checkCollision(*Weapon).collision) {
                if (object->IsSolid) {
                    if (dynamic_cast<PowerArrowObject*>(Weapon)) {
                        dynamic_cast<PowerArrowObject*>(Weapon)->Stuck = true;
                        break;
                    }
                    Weapon->Reset(Player);
                    break;
                }
                SoundEngine->play2D("../resources/audio/solid.wav");
                object->Destroyed = true;
                this->ShouldGeneratePowerUp(*object);
                Weapon->Reset(Player);
            }
        }
    }

    //COLLISION ATTACKERS AND OBJECTS
    for (auto& object : this->Levels[this->Level]->Attackers) {
        if (object->Destroyed || object->pop) {
            continue;
        }
        for (auto& obj : this->Levels[this->Level]->Objects) {
            if (obj->Destroyed || dynamic_cast<LadderObject*>(obj))
                continue;

            Collision& collisionAttackerObj = object->checkCollision(*obj);
            if (collisionAttackerObj.collision) {
                if (collisionAttackerObj.direction == LEFT || collisionAttackerObj.direction == RIGHT)
                {
                    object->Velocity.x = -object->Velocity.x;
                    float penetration = object->Radius - std::abs(collisionAttackerObj.difference.x);
                    if (collisionAttackerObj.direction == LEFT)
                        object->Position.x += penetration;
                    else
                        object->Position.x -= penetration;
                }
                else {
                    object->Velocity.y = -object->Velocity.y;
                    float penetration = object->Radius - std::abs(collisionAttackerObj.difference.y);
                    if (collisionAttackerObj.direction == UP)
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
            if (!wasSleeping) {
                SoundEngine->stopAllSounds();
                this->State = GAME_SLEEP;
                sleepForFrames = 1.0f;
            }
            else {
                Player->Reset(glm::vec3((this->Width - Player->Size.x) / 2.0f, this->Height - Player->Size.y / 2.0f, 0.0f), glm::vec3(500.0f, 500.0f, 0.0f));
                Player->ResetWeapons();
                this->Levels[this->Level]->Reset();
                --Player->Lives;
                wasSleeping = false;

                if (Player->Lives) {
                    SoundEngine->play2D("../resources/audio/stage1.mp3", true);
                    break;
                }
                SoundEngine->play2D("../resources/audio/game-over.mp3");
                break;
            }
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
                PlayerLadderCollision = true;
            }
        }
    }
}

void Game::Reset() {
    Player->Reset(glm::vec3((this->Width - Player->Size.x) / 2.0f, this->Height - Player->Size.y / 2.0f, 0.0f), glm::vec3(500.0f, 500.0f, 0.0f));
    Player->ResetWeapons();
    Player->Lives = lives;
    this->Levels[this->Level]->Reset();
    this->Level = 0;
}

void Game::ShouldGeneratePowerUp(GameObject& object)
{
    //generatePowerUp
    unsigned int r = rand() % 100;
    if (r >= 20 && r <= 30) {
        unsigned int randomPowerUp = rand() % powerups.size();
        this->Levels[this->Level]->PowerUps.push_back(new PowerUpObject(object, ResourceManager::GetTexture(powerups[randomPowerUp]), powerups[randomPowerUp]));
    }
}