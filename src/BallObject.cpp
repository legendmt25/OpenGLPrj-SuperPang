#include "BallObject.h"

BallObject::BallObject(glm::vec2 position, float radius, Texture2D texture, glm::vec3 color, glm::vec2 velocity, glm::vec2 gravity)
	:GameObject(position, glm::vec2(2.0f * radius), texture, color, velocity), Radius(radius), Gravity(gravity) {}

BallObject::BallObject()
	:GameObject(), Radius(12.5f) {}


float clamp(float value, float minn, float maxx) {
	return max(minn, min(maxx, value));
}

Collision BallObject::checkCollision(GameObject& obj) {
	glm::vec2 center(this->Position + this->Radius);
		// calculate AABB info (center, half-extents)
		glm::vec2 rectangle_half_extents(obj.Size.x / 2.0f, obj.Size.y / 2.0f);
		glm::vec2 rectangle_center(obj.Position.x + rectangle_half_extents.x, obj.Position.y + rectangle_half_extents.y);

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

glm::vec2& BallObject::Move(float dt, unsigned int windowWidth, unsigned int windowHeight) {

	this->Position += this->Velocity * dt;
	this->Velocity += this->Gravity;
	if (this->Position.x + this->Size.x > windowWidth || this->Position.x < 0) {
		this->Velocity.x *= -1.0f;
	}
	if (this->Position.y <= 0) {
		this->Velocity.y *= 1.0f;
		this->Position.y = 0;
	}

	if (this->Position.y + this->Size.y >= windowHeight) {
		this->Velocity.y *= -1.0f;
		this->Position.y = windowHeight - this->Size.y;
	}
	return this->Position;
}

void BallObject::Reset(glm::vec2 position, glm::vec2 velocity) {
	this->Position = position;
	this->Velocity = velocity;
}