#pragma once
#include "GameObject.h"
#include "PlayerObject.h"
#include "WeaponObject.h"

class PowerUpObject : public GameObject
{
public:
	std::string Type;

	PowerUpObject();
	PowerUpObject(glm::vec3 position, glm::vec3 size, Texture2D texture, glm::vec3 color = glm::vec3(1.0f), glm::vec3 velocity = glm::vec3(0.0f));
	PowerUpObject(GameObject& spawnedFrom, Texture2D texture, std::string type);
	glm::vec3& Move(float dt, unsigned int windowWidth, unsigned int windowHeight);
	void Activate(PlayerObject* Player);
};