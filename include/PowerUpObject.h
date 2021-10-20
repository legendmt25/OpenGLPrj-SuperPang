#pragma once
#include "GameObject.h"

class PowerUpObject : public GameObject
{
public:
	std::string Type;
	bool Activated;


	PowerUpObject();
	PowerUpObject(glm::vec3 position, glm::vec3 size, Texture2D texture, glm::vec3 color = glm::vec3(1.0f), glm::vec3 velocity = glm::vec3(0.0f));
	PowerUpObject(GameObject& spawnedFrom, Texture2D texture);
};