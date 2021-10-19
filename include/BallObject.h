#pragma once
#include <glm/glm.hpp>

#include "GameObject.h"

class BallObject: public GameObject
{
public:
	glm::vec3 Gravity;
	float Radius;

	BallObject();
	BallObject(glm::vec3 position, float radius, Texture2D texture, glm::vec3 color = glm::vec3(1.0f), glm::vec3 velocity = glm::vec3(0.0f), glm::vec3 gravity = glm::vec3(0.0f, 2.0f, 0.0f));

	Collision checkCollision(GameObject&);
	glm::vec3& Move(float dt, unsigned int window_width, unsigned int window_height);
	void Reset(glm::vec3 position, glm::vec3 velocity);
};