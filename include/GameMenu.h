#pragma once
#include <vector>
#include <list>
#include <fstream>
#include <sstream>
#include <string>

#include "TextRenderer.h"

class Option {
public:
	Option* ParentOption;
	std::string Value;
	glm::vec2 Position;
	float FontSize;
	std::vector<Option*> Options;
	unsigned int Selected;


	Option(std::string Value, glm::vec2 Position = glm::vec2(0.0f), float FontSize = 12.0f);
	~Option();

	void Draw(TextRenderer& Renderer);
};

class GameMenu
{
public:
	std::string Path;
	Option* Selected;

	GameMenu(std::string path);
	~GameMenu();

	void Load(unsigned int windowWidth, unsigned int windowHeight);
	void Draw(TextRenderer& Renderer);
private:
	void loadMenuFromFile(unsigned int windowWidth, unsigned int windowHeight, std::vector<Option*>& Options, Option* Parent);
	void GameMenu::DestroyRecursive(std::vector<Option*>& Options);
};