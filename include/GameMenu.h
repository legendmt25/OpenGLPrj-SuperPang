#pragma once
#include <vector>
#include <list>
#include "TextRenderer.h"
#include <fstream>
#include <sstream>
#include <string>

class Option {
public:
	std::string Value;
	glm::vec2 Position;
	bool Selected;
	float FontSize;
	std::vector<Option> Options;
	
	Option(std::string Value, glm::vec2 Position = glm::vec2(0.0f), float FontSize = 12.0f, std::list<Option> Options = {});
	void ParseOption(std::string option);
	void Draw(TextRenderer& Renderer);
};

class GameMenu
{
public:
	std::vector<Option> Options;
	unsigned int Selected;

	GameMenu() : Selected(0) {}
	GameMenu(std::vector<Option> Options);
	void Load(const char* gameMenuFile, unsigned int windowWidth, unsigned int windowHeight);
	void Draw(TextRenderer& Renderer);
private:
	void loadMenuFromFile(const char* gameMenuFile, unsigned int windowWidth, unsigned int windowHeight);
};