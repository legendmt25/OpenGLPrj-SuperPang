#include "PlayerObject.h"
#include "ResourceManager.h"
#include <Game.h>

PlayerObject::PlayerObject()
    :GameObject() {}

PlayerObject::PlayerObject(glm::vec3 position, glm::vec3 size, Texture2D texture, glm::vec3 color, glm::vec3 velocity)
    : GameObject(position, size, texture, color, velocity)
{
    this->Weapons.push_back(new ArrowObject(*this, glm::vec3(500.0f)));
}

void PlayerObject::Reset(glm::vec3 position, glm::vec3 velocity) {
    for (auto& Weapon : this->Weapons) {
        delete Weapon;
    }
    this->Weapons.clear();

    this->Position = position;
    this->Velocity = velocity;
    this->Weapons.push_back(new ArrowObject(*this, glm::vec3(500.0f)));
}

void PlayerObject::ResetWeapons() {
    for (auto& Weapon : this->Weapons) {
        Weapon->Reset(this);
    }
}

glm::vec3& PlayerObject::Move(float dt, unsigned int windowWidth, unsigned int windowHeight)
{
    if (this->Position.y < windowHeight - this->Size.y / 2.0f) {
        this->Position.y += this->Velocity.y * dt;
    }
    return this->Position;
}

void PlayerObject::Shoot() {
    for (auto& Weapon : this->Weapons) {
        if (!Weapon->Using) {
            Weapon->Reset(this);
            Weapon->UseWeapon();
            break;
        }
    }
}

bool PlayerObject::PlayerAttackerCollision(GameObject& obj) {
    glm::vec2 center(obj.Position);
    // calculate AABB info (center, half-extents)
    glm::vec2 rectangle_half_extents(this->Size.x / 3.5f, this->Size.y / 3.5f);
    //glm::vec2 rectangle_half_extents(0.0f);
    glm::vec2 rectangle_center(this->Position.x, this->Position.y);

    glm::vec2 difference = center - rectangle_center;
    glm::vec2 clamped = glm::clamp(difference, -rectangle_half_extents, rectangle_half_extents);
    // add clamped value to AABB_center and we get the value of box closest to circle
    glm::vec2 closest = rectangle_center + clamped;

    difference = closest - center;

    return glm::length(difference) <= obj.Size.x / 2.0f;
}

float frames = 0;
int PlayerTexture = 1;
float framesUp = 0;
int PlayerUpTexture = 1;

void PlayerObject::ProcessInput(float dt, unsigned int window_width, unsigned int window_height, bool collided) {
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

    if (Game::Keys[GLFW_KEY_RIGHT] || Game::Keys[GLFW_KEY_LEFT]) {
        if (frames >= 0.1f) {
            frames = 0;
            PlayerTexture = (PlayerTexture) % 4 + 1;
        }
        else {
            frames += dt;
        }
        this->Texture = ResourceManager::GetTexture("character-walk" + direction + std::to_string(PlayerTexture));
    }

    if (Game::Keys[GLFW_KEY_SPACE] || Game::Keys[GLFW_KEY_X]) {
        this->Texture = ResourceManager::GetTexture("character-shoot");
    }

    if (Game::Keys[GLFW_KEY_UP] && collided) {
        if (collided && this->Position.y > this->Size.y / 2.0f) {
            this->Position.y -= this->Velocity.y * dt;
        }

        if (framesUp >= 0.1f) {
            framesUp = 0;
            PlayerUpTexture = (PlayerUpTexture) % 3 + 1;
        }
        else {
            framesUp += dt;
        }
        this->Texture = ResourceManager::GetTexture("character-up-" + std::to_string(PlayerUpTexture));
    }

    if (Game::Keys[GLFW_KEY_DOWN] && collided) {
        if (collided && this->Position.y < window_height - this->Size.y / 2.0f) {
            this->Position.y += this->Velocity.y * dt;
        }
        else {
            this->Texture = ResourceManager::GetTexture("character-init");
        }

        if (framesUp >= 0.1f) {
            framesUp = 0;
            PlayerUpTexture = (PlayerUpTexture) % 3 + 1;
            this->Texture = ResourceManager::GetTexture("character-up-" + std::to_string(PlayerUpTexture));
        }
        else {
            framesUp += dt;
        }
    }

    if ((Game::Keys[GLFW_KEY_SPACE] && !Game::KeysProcessed[GLFW_KEY_SPACE]) || (Game::Keys[GLFW_KEY_X] && !Game::KeysProcessed[GLFW_KEY_X])) {
        this->Texture = ResourceManager::GetTexture("character-shoot");
        this->Shoot();
        Game::KeysProcessed[GLFW_KEY_SPACE] = true;
        Game::KeysProcessed[GLFW_KEY_X] = true;
    }

    if (!Game::Keys[GLFW_KEY_RIGHT] && !Game::Keys[GLFW_KEY_LEFT] && !Game::Keys[GLFW_KEY_SPACE] && !Game::Keys[GLFW_KEY_X] && (!Game::Keys[GLFW_KEY_UP] || !collided) && (!Game::Keys[GLFW_KEY_DOWN] || !collided)) {
        this->Texture = ResourceManager::GetTexture("character-init");
        PlayerTexture = 1;
        PlayerUpTexture = 1;
        frames = 0;
        framesUp = 0;
    }
}

PlayerObject::~PlayerObject() {
    for (auto& Weapon : this->Weapons) {
        delete Weapon;
    }
    Weapons.clear();
}