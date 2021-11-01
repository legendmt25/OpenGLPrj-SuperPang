#pragma once
#include <vector>
#include <list>
#include <fstream>
#include <sstream>
#include <string>

#include "Utility.h"
#include "TextRenderer.h"

class OptionValue {
protected:
	int value;
public:
	OptionValue(int value = 0) :value(value) {}
	virtual void Action() = 0;
	virtual int& getValue() {
		return this->value;
	}
};


class CounterValue : public OptionValue {
	int from, to;
public:
	CounterValue(int from, int to) : OptionValue(from), from(from), to(to) {}
	void Action() {
		++this->value;
		if (this->value > this->to) {
			this->value = this->from;
		}
	}
};


class Option {
public:
	Option* ParentOption;
	std::string Name;
	OptionValue* Value;
	glm::vec2 Position;
	float FontSize;
	std::vector<Option*> Options;
	unsigned int Selected;

	Option(std::string Value, glm::vec2 Position = glm::vec2(0.0f), float FontSize = 12.0f, OptionValue* value = nullptr);
	~Option();

	static Option* ParseOption(glm::vec2 position, std::string strOption);
	void Draw(TextRenderer& Renderer);
};

class GameMenu
{
public:
	std::string Path;
	Option* Selected;

	GameMenu(std::string path);
	~GameMenu();

	Option* findOptionByName(std::string optionName);
	void Load(unsigned int windowWidth, unsigned int windowHeight);
	void Draw(TextRenderer& Renderer);
private:
	void loadMenuFromFile(unsigned int windowWidth, unsigned int windowHeight, std::vector<Option*>& Options, Option* Parent);
	Option* GameMenu::findOptionByNameR(std::string optionName, std::vector<Option*> Options);
	void DestroyRecursive(std::vector<Option*>& Options);
};