#pragma once
#include <glm/glm.hpp>
#include <unordered_map>

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

	Collision(bool collision = false, Direction direction = UP, glm::vec2 difference = glm::vec2(0.0f));
};

static std::unordered_map<std::string, float> frameMap;