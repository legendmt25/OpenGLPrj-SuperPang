#include "BallObject.h"
#include "ResourceManager.h"

BallObject::BallObject(glm::vec3 position, float radius, Texture2D texture, glm::vec3 color, glm::vec3 velocity, glm::vec3 gravity)
	:AttackerObject(position, radius, texture, color, velocity), Gravity(gravity) {}

BallObject::BallObject()
	:AttackerObject(), Gravity(0.0f, 1.4f, 0.0f) {}

float framePopBall = 0.0f;
unsigned int PopTextureBall = 1;

void BallObject::Pop(float dt) {
	if (this->pop) {
		if (framePopBall >= 0.01f) {
			this->Texture = ResourceManager::GetTexture("ball-pop-" + std::to_string(PopTextureBall));
			framePopBall = 0.0f;
			++PopTextureBall;
		}
		else {
			framePopBall += dt;
		}
		if (PopTextureBall >= 5) {
			this->Destroyed = true;
			framePopBall = 0.0f;
			PopTextureBall = 1;
		}
	}
}

glm::vec3& BallObject::Move(float dt, unsigned int windowWidth, unsigned int windowHeight) {

	this->Position += this->Velocity * dt;
	this->Velocity += this->Gravity;
	
	if (this->Position.x + this->Size.x / 2.0f > windowWidth || this->Position.x < this->Size.x / 2.0f) {
		this->Velocity.x *= -1.0f;
	}
	if (this->Position.y <= this->Size.y / 2.0f) {
		this->Velocity.y *= 1.0f;
		this->Position.y = this->Size.y / 2.0f;
	}

	if (this->Position.y - this->Size.y / 2.0f >= windowHeight - this->Size.y) {
		this->Velocity.y *= -1.0f;
		this->Position.y = windowHeight - this->Size.y / 2.0f;
		this->Gravity = glm::vec3(0.0f, 1.6f, 0.0f);
	}
	return this->Position;
}

void BallObject::Reset(glm::vec3 position, glm::vec3 velocity) {
	this->Position = position;
	this->Velocity = velocity;
}