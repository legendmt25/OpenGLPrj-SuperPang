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
	void ProcessInput(float dt, unsigned int window_width, unsigned int window_height, bool collided);
	void Shoot();

	bool PlayerAttackerCollision(GameObject&);

	void ResetWeapons();
	glm::vec3& Move(float dt, unsigned int windowWidth, unsigned int windowHeight);
};