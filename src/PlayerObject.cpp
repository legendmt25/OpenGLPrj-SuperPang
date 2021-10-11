#include "PlayerObject.h"
#include "ResourceManager.h"

PlayerObject::PlayerObject()
    :GameObject(), Weapon(nullptr) {}

PlayerObject::PlayerObject(glm::vec2 position, glm::vec2 size, Texture2D texture, glm::vec3 color, glm::vec2 velocity) 
    :GameObject(position, size, texture, color, velocity), 
    Weapon(new WeaponObject(glm::vec2(this->Position.x + this->Size.x / 2.0f, this->Position.y + this->Size.y), glm::vec2(20.0f), ResourceManager::GetTexture("arrow"), glm::vec3(1.0f), glm::vec2(500.0f))) {}

void PlayerObject::Reset(glm::vec2 position, glm::vec2 velocity) {
    this->Position = position;
    this->Velocity = velocity;
}

void PlayerObject::Shoot() {
    this->Weapon->Reset(glm::vec2(this->Position.x + this->Size.x / 2.0f, this->Position.y + this->Size.y), glm::vec2(20.0f));
    this->Weapon->UseWeapon();
}

