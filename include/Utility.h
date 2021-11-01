#pragma once
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>

enum Direction { 
	UP, RIGHT, DOWN, LEFT 
};


Direction VectorDirection(glm::vec2 target);
bool frameCount(const float& dt, float& frame, float toFrame);

class Collision 
{
public:
	bool collision;
	Direction direction;
	glm::vec2 difference;

	Collision(bool collision = false, Direction direction = Direction::UP, glm::vec2 difference = glm::vec2(0.0f));
};

std::vector<std::string> split(std::string str, std::string delimiter);
std::string toLower(std::string str);