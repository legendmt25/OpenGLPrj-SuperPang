#pragma once
#include <glm/glm.hpp>

#include "GameObject.h"
#include "ResourceManager.h"

class LadderObject : public GameObject
{
public:

	LadderObject();
	LadderObject(glm::vec3 position, glm::vec3 size, Texture2D texture = ResourceManager::GetTexture("ladder"), glm::vec3 color = glm::vec3(1.0f), glm::vec3 velocity = glm::vec3(0.0f));
};

