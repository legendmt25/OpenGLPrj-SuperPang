#include "PlayerObject.h"
#include "ResourceManager.h"
#include <Game.h>

PlayerObject::PlayerObject()
    :GameObject() {}

PlayerObject::PlayerObject(glm::vec3 position, glm::vec3 size, Texture2D texture, glm::vec3 color, glm::vec3 velocity)
    : GameObject(position, size, texture, color, velocity)
{
    this->Weapons.push_back(new ArrowObject(*this, glm::vec3(500.0f)));
    this->Weapons.push_back(new PowerArrowObject(*this, glm::vec3(500.0f)));
}

void PlayerObject::Reset(glm::vec3 position, glm::vec3 velocity) {
    this->Position = position;
    this->Velocity = velocity;
}

void PlayerObject::ResetWeapons() {
    for (auto& Weapon : this->Weapons) {
        Weapon->Reset();
    }
}

void PlayerObject::Shoot() {
    for (auto& Weapon : this->Weapons) {
        if (!Weapon->Using) {
            Weapon->Reset();
            Weapon->UseWeapon();
            break;
        }
    }
}

float frames = 0;
int PlayerTexture = 1;

void PlayerObject::ProcessInput(float dt, unsigned int window_width, unsigned int window_height) {
    std::string direction = "";

    if (Game::Keys[GLFW_KEY_LEFT]) {
        if (this->Position.x >= this->Size.x / 2.0f) {
            this->Position.x -= this->Velocity.x * dt;
        }
        direction = "-left-";
    }

    if (Game::Keys[GLFW_KEY_RIGHT]) {
        if (this->Position.x <= window_width - this->Size.x / 2.0f) {
            this->Position.x += this->Velocity.x * dt;
        }
        direction = "-right-";
    }

    if (!Game::Keys[GLFW_KEY_RIGHT] && !Game::Keys[GLFW_KEY_LEFT]) {
        this->Texture = ResourceManager::GetTexture("character-init");
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
        this->Texture = ResourceManager::GetTexture("character-walk" + direction + std::to_string(PlayerTexture));
    }

    if ((Game::Keys[GLFW_KEY_SPACE] && !Game::KeysProcessed[GLFW_KEY_SPACE]) || (Game::Keys[GLFW_KEY_X] && !Game::KeysProcessed[GLFW_KEY_X])) {
        this->Texture = ResourceManager::GetTexture("character-shoot");
        this->Shoot();
        Game::KeysProcessed[GLFW_KEY_SPACE] = true;
        Game::KeysProcessed[GLFW_KEY_X] = true;
    }
}

PlayerObject::~PlayerObject() {
    for (auto& Weapon : this->Weapons) {
        delete Weapon;
    }
}

