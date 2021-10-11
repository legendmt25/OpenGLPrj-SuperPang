#pragma once
#include "WeaponObject.h"

WeaponObject::WeaponObject()
	: GameObject(), Using(false) {}
WeaponObject::WeaponObject(glm::vec2 position, glm::vec2 size, Texture2D texture, glm::vec3 color, glm::vec2 velocity)
	: GameObject(position, size, texture, color, velocity), Using(false) {}

void WeaponObject::UseWeapon() {
	Using = true;
}

glm::vec2& WeaponObject::Move(float dt, unsigned int windowWidth, unsigned int windowHeight) {
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

void WeaponObject::Reset(glm::vec2 position, glm::vec2 size) {
	this->Position = position;
	this->Size = size;
}