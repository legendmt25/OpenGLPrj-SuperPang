#include "LadderObject.h"

LadderObject::LadderObject()
    : GameObject(glm::vec3(0.0f), glm::vec3(1.0f), Texture2D()) {}

LadderObject::LadderObject(glm::vec3 pos, glm::vec3 size, Texture2D texture, glm::vec3 color, glm::vec3 velocity)
    : GameObject(pos, size, texture, color, velocity) {}
