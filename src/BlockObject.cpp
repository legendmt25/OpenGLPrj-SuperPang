#pragma once
#include "BlockObject.h"

BlockObject::BlockObject()
	:GameObject() {}

BlockObject::BlockObject(glm::vec3 position, glm::vec3 size, Texture2D texture, glm::vec3 color, glm::vec3 velocity)
	: GameObject(position, size, texture, color, velocity) {}
