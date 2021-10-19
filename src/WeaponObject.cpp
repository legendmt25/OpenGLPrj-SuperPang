#include "WeaponObject.h"
#include "ResourceManager.h"

WeaponObject::WeaponObject()
	: GameObject(), Using(false), Player(nullptr) {}

WeaponObject::WeaponObject(GameObject& Player, Texture2D texture, glm::vec3 velocity)
	: WeaponObject(glm::vec3(Player.Position.x, Player.Position.y, 0.0f), glm::vec3(20.0f, 20.0f, 1.0f), texture, glm::vec3(1.0f), velocity)
{
	this->Player = &Player;
}

WeaponObject::WeaponObject(glm::vec3 position, glm::vec3 size, Texture2D texture, glm::vec3 color, glm::vec3 velocity)
	: GameObject(position, size, texture, color, velocity), Using(false), Player(nullptr) {}

void WeaponObject::UseWeapon() {
	this->Using = true;
}

void WeaponObject::Reset() {
	this->Position = glm::vec3(Player->Position.x, Player->Position.y + Player->Size.y / 2.0f, 0.0f);
	this->Size = glm::vec3(20.0f, 20.0f, 1.0f);
}

ArrowObject::ArrowObject()
	: WeaponObject() {}

ArrowObject::ArrowObject(GameObject& Player, glm::vec3 velocity)
	: WeaponObject(Player, ResourceManager::GetTexture("arrow"), velocity) {}

ArrowObject::ArrowObject(glm::vec3 position, glm::vec3 size, Texture2D texture, glm::vec3 color, glm::vec3 velocity)
	: WeaponObject(position, size, texture, color, velocity) {}

glm::vec3& ArrowObject::Move(float dt, unsigned int windowWidth, unsigned int windowHeight) {
	if (Using) {
		if (this->Position.y > this->Size.y / 2.0f) {
			this->Position.y -= this->Velocity.y * dt;
			this->Size.y += 2 * this->Velocity.y * dt;
		}
		else {
			this->Using = false;
		}
	}
	return this->Position;
}

PowerArrowObject::PowerArrowObject()
	: WeaponObject() {}

PowerArrowObject::PowerArrowObject(GameObject& Player, glm::vec3 velocity)
	: WeaponObject(Player, ResourceManager::GetTexture("arrow"), velocity) {}

PowerArrowObject::PowerArrowObject(glm::vec3 position, glm::vec3 size, Texture2D texture, glm::vec3 color, glm::vec3 velocity)
	: WeaponObject(position, size, texture, color, velocity) {}

float framesC1 = 0.0f;

glm::vec3& PowerArrowObject::Move(float dt, unsigned int windowWidth, unsigned int windowHeight) {
	if (Using) {
		if (this->Position.y > this->Size.y / 2.0f) {
			this->Position.y -= this->Velocity.y * dt;
			this->Size.y += 2 * this->Velocity.y * dt;
		}
		else {
			if (framesC1 < 2.0f) {
				framesC1 += dt;
			}
			else {
				this->Using = false;
				framesC1 = 0;
			}
		}
	}
	return this->Position;
}
