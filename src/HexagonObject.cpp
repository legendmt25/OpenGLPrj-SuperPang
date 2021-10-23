#pragma once
#include "HexagonObject.h"
#include "ResourceManager.h"

HexagonObject::HexagonObject()
	:AttackerObject() {}

HexagonObject::HexagonObject(glm::vec3 position, glm::vec3 size, Texture2D texture, glm::vec3 color, glm::vec3 velocity)
	: AttackerObject(position, size.x / 2.0f, texture, color, velocity) {}

unsigned int HexagonTexture = 1;

glm::vec3& HexagonObject::Move(float dt, unsigned int windowWidth, unsigned int windowHeight) {
	//std::cout << HexagonTexture << std::endl;
	if (frameCount(dt, frameMap["hexagon"], 0.1f)) {
		this->Texture = ResourceManager::GetTexture("hexagon-" + std::to_string(HexagonTexture));
		HexagonTexture = (HexagonTexture) % 3 + 1;
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

unsigned int PopTextureHexagon = 1;

void HexagonObject::Pop(float dt) {
	if (this->pop) {
		if (PopTextureHexagon < 5) {
			if (frameCount(dt, frameMap["pop-hexagon"], 0.005)) {
				this->Texture = ResourceManager::GetTexture("hexagon-pop-" + std::to_string(PopTextureHexagon));
				++PopTextureHexagon;
			}
		}
		else {
			this->Destroyed = true;
			PopTextureHexagon = 1;
		}
	}
}
