#pragma once
#include "GameObject.h"
#include "PowerUpObject.h"

class BlockObject : public GameObject
{
public:

	BlockObject();
	BlockObject(glm::vec3 position, glm::vec3 size, Texture2D texture, glm::vec3 color = glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3 velocity = glm::vec3(0.0f));
};