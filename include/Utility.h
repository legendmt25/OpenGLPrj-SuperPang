#pragma once
#include <tuple>
#include <glm/glm.hpp>


enum Direction { 
	UP, RIGHT, DOWN, LEFT 
};

typedef std::tuple<bool, Direction, glm::vec2> Collision;

Direction VectorDirection(glm::vec2 target);