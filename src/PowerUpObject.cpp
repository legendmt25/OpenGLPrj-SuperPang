#pragma once
#include "PowerUpObject.h"

PowerUpObject::PowerUpObject()
	:GameObject() {}

PowerUpObject::PowerUpObject(glm::vec3 position, glm::vec3 size, Texture2D texture, glm::vec3 color, glm::vec3 velocity)
	: GameObject(position, size, texture, color, velocity)
{
	this->frame["DestroyPowerUp"] = 0.0f;
}

PowerUpObject::PowerUpObject(GameObject& spawnedFrom, Texture2D texture, std::string type)
	:GameObject(spawnedFrom.Position, glm::vec3(30.0f, 30.0f, 1.0f), texture, glm::vec3(1.0f), glm::vec3(0.0f, 300.0f, 0.0f)), Type(type) {}

glm::vec3& PowerUpObject::Move(float dt, unsigned int windowWidth, unsigned int windowHeight) {
	if (this->Position.y < windowHeight - this->Size.y / 2.0f) {
		this->Position.y += this->Velocity.y * dt;
	}
	else if (frameCount(dt, this->frame["DestroyPowerUp"], 10.0f)) {
		this->Destroyed = true;
	}
	return this->Position;
}

void PowerUpObject::Activate(PlayerObject* Player) {
	if (this->Type == "double-arrow") {
		Player->Weapons.clear();
		Player->Weapons.push_back(new ArrowObject(*Player, glm::vec3(500.0f)));
		Player->Weapons.push_back(new ArrowObject(*Player, glm::vec3(500.0f)));
	}
	if (this->Type == "power-arrow") {
		Player->Weapons.clear();
		Player->Weapons.push_back(new PowerArrowObject(*Player, glm::vec3(500.0f)));
	}
}