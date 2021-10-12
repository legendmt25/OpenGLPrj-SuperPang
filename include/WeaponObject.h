#pragma once
#include <glm/glm.hpp>

#include "GameObject.h"

class WeaponObject: public GameObject
{
	GameObject* Player;
public:
	bool Using;

	WeaponObject();
	WeaponObject(GameObject& Player, Texture2D texture, glm::vec2 velocity);
	WeaponObject(glm::vec2 position, glm::vec2 size, Texture2D texture, glm::vec3 color = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.0f));
	void UseWeapon();
	virtual glm::vec2& Move(float dt, unsigned int windowWidth, unsigned int windowHeight) = 0;
	void Reset();
};

class ArrowObject: public WeaponObject 
{
public:
	ArrowObject();
	ArrowObject(GameObject& Player, glm::vec2 velocity);
	ArrowObject(glm::vec2 position, glm::vec2 size, Texture2D texture, glm::vec3 color = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.0f));
	glm::vec2& Move(float dt, unsigned int windowWidth, unsigned int windowHeight);
};

class PowerArrowObject : public WeaponObject {
public:
	PowerArrowObject();
	PowerArrowObject(GameObject& Player, glm::vec2 velocity);
	PowerArrowObject(glm::vec2 position, glm::vec2 size, Texture2D texture, glm::vec3 color = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.0f));
	glm::vec2& Move(float dt, unsigned int windowWidth, unsigned int windowHeight);
};