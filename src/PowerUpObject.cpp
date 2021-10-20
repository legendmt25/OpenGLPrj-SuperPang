#pragma once
#include "PowerUpObject.h"

PowerUpObject::PowerUpObject()
	:GameObject() {}

PowerUpObject::PowerUpObject(glm::vec3 position, glm::vec3 size, Texture2D texture, glm::vec3 color, glm::vec3 velocity)
	: GameObject(position, size, texture, color, velocity) {}

PowerUpObject::PowerUpObject(GameObject& spawnedFrom, Texture2D texture)
	:GameObject(spawnedFrom.Position, glm::vec3(20.0f, 20.0f, 1.0f), texture, glm::vec3(1.0f), glm::vec3(0.0f, 100.0f, 0.0f)) {}