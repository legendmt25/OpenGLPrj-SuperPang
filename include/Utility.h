#pragma once
#include <glm/glm.hpp>


enum Direction { 
	UP, RIGHT, DOWN, LEFT 
};


Direction VectorDirection(glm::vec2 target);

class Collision 
{
public:
	bool collision;
	Direction direction;
	glm::vec2 difference;

	Collision(bool collision = false, Direction direction = UP, glm::vec2 difference = glm::vec2(0.0f));
};