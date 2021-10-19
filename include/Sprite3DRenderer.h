#pragma once
#include "Shader.h"
#include "Texture.h"

class Sprite3DRenderer 
{
public:
	Sprite3DRenderer(Shader& shader);
	~Sprite3DRenderer();

	void DrawSprite(Texture2D& texture, glm::vec3 position, glm::vec3 size, float rotate, glm::vec3 color);

private:
	Shader shader;
	unsigned int QuadVAO;

	void initRenderData();
};