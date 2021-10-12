#include "PlayerObject.h"
#include "ResourceManager.h"

PlayerObject::PlayerObject()
    :GameObject() {}

PlayerObject::PlayerObject(glm::vec2 position, glm::vec2 size, Texture2D texture, glm::vec3 color, glm::vec2 velocity)
    : GameObject(position, size, texture, color, velocity)
{
    this->Weapons.push_back(new ArrowObject(*this, glm::vec2(500.0f)));
    this->Weapons.push_back(new PowerArrowObject(*this, glm::vec2(500.0f)));
}

void PlayerObject::Reset(glm::vec2 position, glm::vec2 velocity) {
    this->Position = position;
    this->Velocity = velocity;
}

void PlayerObject::Shoot() {
    for (auto& Weapon : this->Weapons) {
        if (!Weapon->Using) {
            Weapon->UseWeapon();
            break;
        }
    }
}

PlayerObject::~PlayerObject() {
    for (auto& Weapon : this->Weapons) {
        delete Weapon;
    }
}

