#include "AttackerObject.h"
#include "ResourceManager.h"

AttackerObject::AttackerObject(glm::vec3 position, float radius, Texture2D texture, glm::vec3 color, glm::vec3 velocity)
	:GameObject(position, glm::vec3(2 * radius, 2 * radius, 1.0f), texture, color, velocity), Radius(radius), pop(false) {}

AttackerObject::AttackerObject(glm::vec3 position, glm::vec3 size, Texture2D texture, glm::vec3 color, glm::vec3 velocity)
	:AttackerObject(position, size.x, texture, color, velocity) {}

AttackerObject::AttackerObject()
	:GameObject(), Radius(12.5f), pop(false) {}

void AttackerObject::Draw(SphereRenderer& renderer) {
	renderer.DrawSphere(this->Texture, this->Position, glm::vec3(this->Radius, this->Radius, 1.0f), this->Rotation, this->Color);
}

Collision AttackerObject::checkCollision(GameObject& obj) {
	glm::vec2 center(this->Position);
	// calculate AABB info (center, half-extents)
	glm::vec2 rectangle_half_extents(obj.Size.x / 2.0f, obj.Size.y / 2.0f);
	//glm::vec2 rectangle_half_extents(0.0f);
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


void AttackerObject::Reset(glm::vec3 position, glm::vec3 velocity) {
	this->Position = position;
	this->Velocity = velocity;
}