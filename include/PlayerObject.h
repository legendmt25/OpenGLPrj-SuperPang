#pragma once
#include <glm/glm.hpp>
#include <list>

#include "WeaponObject.h"

class PlayerObject: public GameObject
{
public:
	std::list<WeaponObject*> Weapons;

	PlayerObject();
	~PlayerObject();
	PlayerObject(glm::vec3 position, glm::vec3 size, Texture2D texture, glm::vec3 color = glm::vec3(1.0f), glm::vec3 velocity = glm::vec3(0.0f));
	void Reset(glm::vec3 position, glm::vec3 velocity);
	void Shoot();
};