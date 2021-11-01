#pragma once
#include "AttackerObject.h"

class HexagonObject : public AttackerObject
{
public:
	HexagonObject();
	HexagonObject(glm::vec3 position, glm::vec3 size, Texture2D texture, glm::vec3 color = glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3 velocity = glm::vec3(0.0f));

	glm::vec3& Move(float dt, unsigned int window_width, unsigned int window_height);
	void Reset(glm::vec3 position, glm::vec3 velocity);
	void Pop(float dt);

private:
	float frameRotate;
};