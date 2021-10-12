#include "WeaponObject.h"
#include "ResourceManager.h"

WeaponObject::WeaponObject()
	: GameObject(), Using(false) {}

WeaponObject::WeaponObject(GameObject& Player, Texture2D texture, glm::vec2 velocity)
	: WeaponObject(glm::vec2(Player.Position.x + Player.Size.x / 2.0f, Player.Position.y - Player.Size.y), glm::vec2(20.0f), texture, glm::vec3(1.0f), velocity)
{
	this->Player = &Player;
}

WeaponObject::WeaponObject(glm::vec2 position, glm::vec2 size, Texture2D texture, glm::vec3 color, glm::vec2 velocity)
	: GameObject(position, size, texture, color, velocity), Using(false) {}

void WeaponObject::UseWeapon() {
	Using = true;
}

void WeaponObject::Reset() {
	this->Position = glm::vec2(Player->Position.x + Player->Size.x / 2.0f, Player->Position.y + Player->Size.y);
	this->Size = glm::vec2(20.0f);
}

ArrowObject::ArrowObject()
	: WeaponObject() {}

ArrowObject::ArrowObject(GameObject& Player, glm::vec2 velocity)
	: WeaponObject(Player, ResourceManager::GetTexture("arrow"), velocity) {}

ArrowObject::ArrowObject(glm::vec2 position, glm::vec2 size, Texture2D texture, glm::vec3 color, glm::vec2 velocity)
	: WeaponObject(position, size, texture, color, velocity) {}

glm::vec2& ArrowObject::Move(float dt, unsigned int windowWidth, unsigned int windowHeight) {
	if (Using) {
		if (this->Position.y > 0) {
			this->Position.y -= this->Velocity.y * dt;
			this->Size.y += this->Velocity.y * dt;
		}
		else {
			this->Using = false;
		}
	}
	return this->Position;
}

PowerArrowObject::PowerArrowObject()
	: WeaponObject() {}

PowerArrowObject::PowerArrowObject(GameObject& Player, glm::vec2 velocity)
	: WeaponObject(Player, ResourceManager::GetTexture("arrow"), velocity) {}

PowerArrowObject::PowerArrowObject(glm::vec2 position, glm::vec2 size, Texture2D texture, glm::vec3 color, glm::vec2 velocity)
	: WeaponObject(position, size, texture, color, velocity) {}

float framesC1 = 0.0f;

glm::vec2& PowerArrowObject::Move(float dt, unsigned int windowWidth, unsigned int windowHeight) {
	if (Using) {
		if (this->Position.y > 0) {
			this->Position.y -= this->Velocity.y * dt;
			this->Size.y += this->Velocity.y * dt;
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
