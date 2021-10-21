#pragma once
#include "HexagonObject.h"
#include "ResourceManager.h"

#include <iostream>

HexagonObject::HexagonObject()
	:BallObject() {}

HexagonObject::HexagonObject(glm::vec3 position, glm::vec3 size, Texture2D texture, glm::vec3 color, glm::vec3 velocity)
	: BallObject(position, size.x / 2.0f, texture, color, velocity) {}

unsigned int HexagonTexture = 1;
float frameHexagon = 0.0f;

glm::vec3& HexagonObject::Move(float dt, unsigned int windowWidth, unsigned int windowHeight) {
	std::cout << HexagonTexture << std::endl;
	if (frameHexagon > 0.1f) {
		this->Texture = ResourceManager::GetTexture("hexagon-" + std::to_string(HexagonTexture));
		frameHexagon = 0.0f;
		HexagonTexture = (HexagonTexture) % 3 + 1;
	}
	else {
		frameHexagon += dt;
	}

	this->Position += this->Velocity * dt;

	if (this->Position.x + this->Size.x / 2.0f > windowWidth || this->Position.x < this->Size.x / 2.0f) {
		this->Velocity.x = -this->Velocity.x;
	}
	if (this->Position.y <= this->Size.y / 2.0f) {
		this->Velocity.y = -this->Velocity.y;
		this->Position.y = this->Size.y / 2.0f;
	}

	if (this->Position.y - this->Size.y / 2.0f >= windowHeight - this->Size.y) {
		this->Velocity.y = -this->Velocity.y;
		this->Position.y = windowHeight - this->Size.y / 2.0f;
	}
	return this->Position;
}

void HexagonObject::Reset(glm::vec3 position, glm::vec3 velocity) {
	this->Position = position;
	this->Velocity = velocity;
}

float framePopHexagon = 0.0f;
unsigned int PopTextureHexagon = 1;

void HexagonObject::Pop(float dt) {
	if (this->pop) {
		if (framePopHexagon >= 0.01f) {
			this->Texture = ResourceManager::GetTexture("hexagon-pop-" + std::to_string(PopTextureHexagon));
			framePopHexagon = 0.0f;
			++PopTextureHexagon;
		}
		else {
			framePopHexagon += dt;
		}
		if (PopTextureHexagon >= 5) {
			this->Destroyed = true;
			framePopHexagon = 0.0f;
			PopTextureHexagon = 1;
		}
	}
}
