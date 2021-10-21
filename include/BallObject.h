#pragma once
#include <glm/glm.hpp>

#include "AttackerObject.h"

class BallObject: public AttackerObject
{
public:
	glm::vec3 Gravity;

	BallObject();
	BallObject(glm::vec3 position, float radius, Texture2D texture, glm::vec3 color = glm::vec3(1.0f), glm::vec3 velocity = glm::vec3(0.0f), glm::vec3 gravity = glm::vec3(0.0f, 1.4f, 0.0f));

	glm::vec3& Move(float dt, unsigned int window_width, unsigned int window_height);
	void Reset(glm::vec3 position, glm::vec3 velocity);
	void Pop(float dt);
};