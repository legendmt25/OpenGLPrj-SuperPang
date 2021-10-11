#pragma once
#include <vector>
#include <list>
#include "TextRenderer.h"

class Option {
public:
	std::string Value;
	glm::vec2 Position;
	bool Selected;
	float FontSize;
	std::list<Option> Options;
	
	Option(std::string Value, glm::vec2 Position = glm::vec2(0.0f), float FontSize = 12.0f, std::list<Option> Options = {});
	void AlignCenter(unsigned int Width, unsigned int Height, float HeightOffset = 0.0f);
	void Draw(TextRenderer& Renderer);
};

class GameMenu
{
public:
	std::vector<Option> Options;
	unsigned int Selected;

	GameMenu() : Selected(0) {}
	GameMenu(std::vector<Option> Options);
	void Draw(TextRenderer& Renderer);
};