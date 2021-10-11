#pragma once
#include <glm/glm.hpp>

#include "Texture.h"
#include "SpriteRenderer.h"
#include "Utility.h"

class GameObject
{
public:
	glm::vec2 Position, Size, Velocity;
	glm::vec3  Color;
	float Rotation;
	bool IsSolid;
	bool Destroyed;
	Texture2D Texture;

	GameObject();
	GameObject(glm::vec2 position, glm::vec2 size, Texture2D texture, glm::vec3 color = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.0f));
	virtual Collision checkCollision(GameObject&);
	virtual void Draw(SpriteRenderer& renderer);
	virtual glm::vec2& Move(float dt, unsigned int window_width, unsigned int window_height);
	virtual void Reset();
};