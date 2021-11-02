#include "PlayerObject.h"
#include "ResourceManager.h"
#include "Game.h"

PlayerObject::PlayerObject()
    :GameObject(), Lives(5), Alive(true), UpTexture(1), WalkTexture(1) {}

PlayerObject::PlayerObject(glm::vec3 position, glm::vec3 size, Texture2D texture, glm::vec3 color, glm::vec3 velocity, int playerNumber)
    : GameObject(position, size, texture, color, velocity), Lives(5), Alive(true), UpTexture(1), WalkTexture(1)
{
    this->frame["Up"] = this->frame["Walk"] = 0.0f;
    CollisionWith["block"] = false;
    CollisionWith["ladder"] = false;
    if (playerNumber == 1) {
        playerMovement = PlayerMovement(GLFW_KEY_UP, GLFW_KEY_DOWN, GLFW_KEY_LEFT, GLFW_KEY_RIGHT, GLFW_KEY_SPACE, GLFW_KEY_X);
    }
    else {
        playerMovement = PlayerMovement(GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_A, GLFW_KEY_D, GLFW_KEY_K, GLFW_KEY_L);
    }
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

bool PlayerObject::isAlive() {
    return this->Alive;
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


void PlayerObject::ProcessInput(float dt, unsigned int window_width, unsigned int window_height) {
    std::string direction = "";

    if (Game::Keys[playerMovement.LEFT]) {
        if (this->Position.x >= this->Size.x / 2.0f) {
            this->Position.x -= this->Velocity.x * dt;
        }
        direction = "-left-";
    }

    if (Game::Keys[playerMovement.RIGHT]) {
        if (this->Position.x <= window_width - this->Size.x / 2.0f) {
            this->Position.x += this->Velocity.x * dt;
        }
        direction = "-right-";
    }

    if (Game::Keys[playerMovement.RIGHT] || Game::Keys[playerMovement.LEFT]) {
        if (frameCount(dt, this->frame["Walk"], 0.1f)) {
            WalkTexture = (WalkTexture) % 4 + 1;
        }
        this->Texture = ResourceManager::GetTexture("character-walk" + direction + std::to_string(WalkTexture));
    }

    if (Game::Keys[playerMovement.SHOOT1] || Game::Keys[playerMovement.SHOOT2]) {
        this->Texture = ResourceManager::GetTexture("character-shoot");
    }

    if (Game::Keys[playerMovement.UP] && this->CollisionWith["ladder"]) {
        if (this->CollisionWith["ladder"] && this->Position.y > this->Size.y / 2.0f) {
            this->Position.y -= this->Velocity.y * dt;
        }

        if (frameCount(dt, this->frame["Up"], 0.1f)) {
            UpTexture = (UpTexture) % 3 + 1;
        }
        this->Texture = ResourceManager::GetTexture("character-up-" + std::to_string(UpTexture));
    }

    if (Game::Keys[playerMovement.DOWN] && this->CollisionWith["ladder"]) {
        if (this->CollisionWith["ladder"] && this->Position.y < window_height - this->Size.y / 2.0f) {
            this->Position.y += this->Velocity.y * dt;
        }
        else {
            this->Texture = ResourceManager::GetTexture("character-init");
        }

        if (frameCount(dt, this->frame["Up"], 0.1f)) {
            UpTexture = (UpTexture) % 3 + 1;
            this->Texture = ResourceManager::GetTexture("character-up-" + std::to_string(UpTexture));
        }
    }

    if ((Game::Keys[playerMovement.SHOOT1] && !Game::KeysProcessed[playerMovement.SHOOT1]) || (Game::Keys[playerMovement.SHOOT2] && !Game::KeysProcessed[playerMovement.SHOOT2])) {
        this->Texture = ResourceManager::GetTexture("character-shoot");
        this->Shoot();
        Game::KeysProcessed[playerMovement.SHOOT1] = true;
        Game::KeysProcessed[playerMovement.SHOOT2] = true;
    }

    if (!Game::Keys[playerMovement.RIGHT] && !Game::Keys[playerMovement.LEFT] && !Game::Keys[playerMovement.UP] && !Game::Keys[playerMovement.DOWN] && !Game::Keys[playerMovement.SHOOT1] && !Game::Keys[playerMovement.SHOOT2] && !this->CollisionWith["ladder"]) {
        this->Texture = ResourceManager::GetTexture("character-init");
        WalkTexture = 1;
        UpTexture = 1;
        this->frame["Up"] = this->frame["Walk"] = 0.0f;
    }
}

PlayerObject::~PlayerObject() {
    for (auto& Weapon : this->Weapons) {
        delete Weapon;
    }
    Weapons.clear();
}