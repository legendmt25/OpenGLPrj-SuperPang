#pragma once
#include <glm/glm.hpp>

#include "Texture.h"
#include "SpriteRenderer.h"
#include "Sprite3DRenderer.h"
#include "Utility.h"

class GameObject
{
public:
	glm::vec3 Position, Size, Velocity, Color;
	float Rotation;
	bool IsSolid;
	bool Destroyed;
	Texture2D Texture;

	GameObject();
	virtual ~GameObject() = default;
	GameObject(glm::vec3 position, glm::vec3 size, Texture2D texture, glm::vec3 color = glm::vec3(1.0f), glm::vec3 velocity = glm::vec3(0.0f));
	virtual Collision checkCollision(GameObject&);
	virtual void Draw(Sprite3DRenderer& renderer);
	virtual glm::vec3& Move(float dt, unsigned int window_width, unsigned int window_height);
	virtual void ProcessInput(float dt, unsigned int window_width, unsigned int window_height);
	virtual void Reset();
};