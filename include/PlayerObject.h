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
	PlayerObject(glm::vec2 position, glm::vec2 size, Texture2D texture, glm::vec3 color = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.0f));
	void Reset(glm::vec2 position, glm::vec2 velocity);
	void Shoot();
};