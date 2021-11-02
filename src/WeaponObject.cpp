#include "WeaponObject.h"
#include "ResourceManager.h"

WeaponObject::WeaponObject()
	: GameObject(), Using(false) {}

WeaponObject::WeaponObject(GameObject& Player, Texture2D texture, glm::vec3 velocity)
	: WeaponObject(glm::vec3(Player.Position.x, Player.Position.y + Player.Size.y / 2.0f - 11.0f, 0.0f), glm::vec3(20.0f, 20.0f, 1.0f), texture, glm::vec3(1.0f), velocity)  
{}

WeaponObject::WeaponObject(glm::vec3 position, glm::vec3 size, Texture2D texture, glm::vec3 color, glm::vec3 velocity)
	: GameObject(position, size, texture, color, velocity), Using(false) {}

void WeaponObject::UseWeapon() {
	this->Using = true;
}

void WeaponObject::Reset(GameObject* Player) {
	this->Position = glm::vec3(Player->Position.x, Player->Position.y + Player->Size.y / 2.0f - 11.0f, 0.0f);
	this->Size = glm::vec3(20.0f, 20.0f, 1.0f);
	Using = false;
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
	: WeaponObject(), Stuck(false)
{
	this->frame["StuckPowerArrow"] = 0.0f;
}

PowerArrowObject::PowerArrowObject(GameObject& Player, glm::vec3 velocity)
	: WeaponObject(Player, ResourceManager::GetTexture("arrow"), velocity), Stuck(false)
{
	this->frame["StuckPowerArrow"] = 0.0f;
}

PowerArrowObject::PowerArrowObject(glm::vec3 position, glm::vec3 size, Texture2D texture, glm::vec3 color, glm::vec3 velocity)
	: WeaponObject(position, size, texture, color, velocity), Stuck(false)
{
	this->frame["StuckPowerArrow"] = 0.0f;
}

glm::vec3& PowerArrowObject::Move(float dt, unsigned int windowWidth, unsigned int windowHeight) {
	
	if (this->Using && !this->Stuck) {
		if (this->Position.y > this->Size.y / 2.0f && this->Using) {
			this->Position.y -= this->Velocity.y * dt;
			this->Size.y += 2 * this->Velocity.y * dt;
		}
		else {
			this->Stuck = true;
		}
	}
	if (this->Stuck) {
		if (frameCount(dt, this->frame["StuckPowerArrow"], 2.0f)) {
			this->Using = false;
			this->Stuck = false;
		}
	}
	return this->Position;
}

void PowerArrowObject::Reset(GameObject* Player) {
	WeaponObject::Reset(Player);
	this->Stuck = false;
}
