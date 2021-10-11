#pragma once
#include <glm/glm.hpp>

#include "GameObject.h"

class BallObject: public GameObject
{
public:
	glm::vec2 Gravity;
	float Radius;

	BallObject();
	BallObject(glm::vec2 position, float radius, Texture2D texture, glm::vec3 color = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.0f), glm::vec2 gravity = glm::vec2(0.0f, 2.0f));

	Collision checkCollision(GameObject&);
	glm::vec2& Move(float dt, unsigned int window_width, unsigned int window_height);
	void Reset(glm::vec2 position, glm::vec2 velocity);
};