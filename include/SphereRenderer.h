#pragma once
#include <vector>
#include <glm/glm.hpp>

#include "Shader.h"
#include "Texture.h"

class SphereRenderer
{
public:
	SphereRenderer(Shader& shader, int squares = 18, int layers = 36);
	~SphereRenderer();

	void DrawSphere(Texture2D& texture, glm::vec3 position, glm::vec3 size, float rotate, glm::vec3 color);

private:
	Shader shader;
	unsigned int QuadVAO;
	unsigned int EBO;
	int squares;
	int layers;
	int indicesCount;

	void initRenderData();
};