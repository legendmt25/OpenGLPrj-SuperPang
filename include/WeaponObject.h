#pragma once
#include <glm/glm.hpp>

#include "GameObject.h"

class WeaponObject: public GameObject
{
public:
	bool Using;

	WeaponObject();
	WeaponObject(glm::vec2 position, glm::vec2 size, Texture2D texture, glm::vec3 color = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.0f));
	void UseWeapon();
	glm::vec2& Move(float dt, unsigned int windowWidth, unsigned int windowHeight);
	void Reset(glm::vec2 position, glm::vec2 size);
};