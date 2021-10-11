#pragma once
#include "Shader.h"
#include "Texture.h"

class SpriteRenderer 
{
public:
	SpriteRenderer(Shader& shader);
	~SpriteRenderer();

	void DrawSprite(Texture2D& texture, glm::vec2 position, glm::vec2 size, float rotate, glm::vec3 color);

private:
	Shader shader;
	unsigned int QuadVAO;

	void initRenderData();
};