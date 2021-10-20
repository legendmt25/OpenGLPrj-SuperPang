#pragma once
#include <glm/glm.hpp>

#include "GameObject.h"

class WeaponObject: public GameObject
{
public:
	bool Using;

	WeaponObject();
	WeaponObject(GameObject& Player, Texture2D texture, glm::vec3 velocity);
	WeaponObject(glm::vec3 position, glm::vec3 size, Texture2D texture, glm::vec3 color = glm::vec3(1.0f), glm::vec3 velocity = glm::vec3(0.0f));
	void UseWeapon();
	virtual glm::vec3& Move(float dt, unsigned int windowWidth, unsigned int windowHeight) = 0;
	virtual void Reset(GameObject* Player);
};

class ArrowObject: public WeaponObject 
{
public:
	ArrowObject();
	ArrowObject(GameObject& Player, glm::vec3 velocity);
	ArrowObject(glm::vec3 position, glm::vec3 size, Texture2D texture, glm::vec3 color = glm::vec3(1.0f), glm::vec3 velocity = glm::vec3(0.0f));
	glm::vec3& Move(float dt, unsigned int windowWidth, unsigned int windowHeight);
};

class PowerArrowObject : public WeaponObject {
public:
	bool Stuck;

	PowerArrowObject();
	PowerArrowObject(GameObject& Player, glm::vec3 velocity);
	PowerArrowObject(glm::vec3 position, glm::vec3 size, Texture2D texture, glm::vec3 color = glm::vec3(1.0f), glm::vec3 velocity = glm::vec3(0.0f));
	glm::vec3& Move(float dt, unsigned int windowWidth, unsigned int windowHeight);
	void Reset(GameObject* Player);
};