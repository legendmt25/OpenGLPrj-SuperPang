#include "BallObject.h"
#include "ResourceManager.h"

BallObject::BallObject(glm::vec3 position, float radius, Texture2D texture, glm::vec3 color, glm::vec3 velocity, glm::vec3 gravity)
	:GameObject(position, glm::vec3(2.0f * radius, 2.0f * radius, 1.0f), texture, color, velocity), Radius(radius), Gravity(gravity), pop(false)
{
	this->Position.x += radius;
	this->Position.y += radius;
}

BallObject::BallObject()
	:GameObject(), Radius(12.5f), Gravity(0.0f, 1.4f, 0.0f), pop(false) {}


float clamp(float value, float minn, float maxx) {
	return max(minn, min(maxx, value));
}

Collision BallObject::checkCollision(GameObject& obj) {
	glm::vec2 center(this->Position);
	// calculate AABB info (center, half-extents)
	glm::vec2 rectangle_half_extents(obj.Size.x / 2.0f, obj.Size.y / 2.0f);
	glm::vec2 rectangle_center(obj.Position.x, obj.Position.y);

	glm::vec2 difference = center - rectangle_center;
	glm::vec2 clamped = glm::clamp(difference, -rectangle_half_extents, rectangle_half_extents);
	// add clamped value to AABB_center and we get the value of box closest to circle
	glm::vec2 closest = rectangle_center + clamped;
		
	difference = closest - center;

	if (glm::length(difference) <= this->Radius) {
		return Collision(true, VectorDirection(difference), difference);
	}
	else {
		return Collision();
	}
}

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