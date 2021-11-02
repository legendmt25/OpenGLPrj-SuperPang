#pragma once
#include <vector>
#include <list>
#include <fstream>
#include <sstream>
#include <string>

#include "Utility.h"
#include "TextRenderer.h"

class OptionValue {
public:
	OptionValue() = default;
	virtual void Action() = 0;
	virtual std::string toString() = 0;
};

class OptionWithoutValue: public OptionValue {
	void Action() {
		return;
	}
	virtual std::string toString() {
		return "";
	}
};

template<class T>
class OptionWithValue : public OptionValue {
protected:
	T value;
public:
	OptionWithValue(int value = 0) :value(value) {}
	virtual void Action() = 0;
	virtual std::string toString() = 0;
	virtual T& getValue() {
		return this->value;
	}
};

class CounterValue : public OptionWithValue<int> {
	unsigned short int from, to;
public:
	CounterValue(int from, int to) : OptionWithValue(from), from(from), to(to) {}
	void Action();
	std::string toString() {
		return std::to_string(this->getValue());
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