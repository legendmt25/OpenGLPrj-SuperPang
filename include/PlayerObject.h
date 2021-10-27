#pragma once
#include <glm/glm.hpp>
#include <list>

#include "WeaponObject.h"
#include "Utility.h"

class PlayerMovement {
public:
	unsigned short int UP, DOWN, LEFT, RIGHT, SHOOT1, SHOOT2;
	PlayerMovement(unsigned short int UP = GLFW_KEY_UP, 
				   unsigned short int DOWN = GLFW_KEY_DOWN, 
				   unsigned short int LEFT = GLFW_KEY_LEFT, 
				   unsigned short int RIGHT = GLFW_KEY_RIGHT, 
				   unsigned short int SHOOT1 = GLFW_KEY_SPACE, 
				   unsigned short int SHOOT2 = GLFW_KEY_X)
		:UP(UP), DOWN(DOWN), LEFT(LEFT), RIGHT(RIGHT), SHOOT1(SHOOT1), SHOOT2(SHOOT2) {}
};

class PlayerObject: public GameObject
{
public:
	std::list<WeaponObject*> Weapons;
	unsigned int Lives;

	PlayerObject();
	~PlayerObject();
	PlayerObject(glm::vec3 position, glm::vec3 size, Texture2D texture, glm::vec3 color = glm::vec3(1.0f), glm::vec3 velocity = glm::vec3(0.0f), int playerNumber = 1);
	void Reset(glm::vec3 position, glm::vec3 velocity);
	void ProcessInput(float dt, unsigned int window_width, unsigned int window_height, bool collided);
	void Shoot();

	bool PlayerAttackerCollision(GameObject&);

	void ResetWeapons();
	glm::vec3& Move(float dt, unsigned int windowWidth, unsigned int windowHeight);
private:
	PlayerMovement playerMovement;
};