#pragma once
#include <glm/glm.hpp>

#include "GameObject.h"

class AttackerObject: public GameObject
{
public:
	float Radius;
	bool pop;

	AttackerObject();
	AttackerObject(glm::vec3 position, float radius, Texture2D texture, glm::vec3 color = glm::vec3(1.0f), glm::vec3 velocity = glm::vec3(0.0f));
	AttackerObject(glm::vec3 position, glm::vec3 size, Texture2D texture, glm::vec3 color = glm::vec3(1.0f), glm::vec3 velocity = glm::vec3(0.0f));

	Collision checkCollision(GameObject&);
	virtual glm::vec3& Move(float dt, unsigned int window_width, unsigned int window_height) = 0;
	void Reset(glm::vec3 position, glm::vec3 velocity);
	virtual void Pop(float dt) = 0;
};