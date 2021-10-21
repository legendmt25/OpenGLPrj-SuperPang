#include <GameMenu.h>
#include <iostream>

Option::Option(std::string Value, glm::vec2 Position, float FontSize, std::list<Option> Options)
	:Value(Value), Position(Position), FontSize(FontSize), Selected(false) {}

void Option::Draw(TextRenderer& Renderer) {
	std::string h = "";
	if (this->Selected) {
		h = "->";
	}
	Renderer.RenderText(h + this->Value, this->Position.x, this->Position.y, 1.0f);
}

GameMenu::GameMenu(std::vector<Option> Options)
	:GameMenu()
{
	this->Options = Options;
	if (this->Options.size() > 0) {
		this->Options[0].Selected = true;
	}
}

void GameMenu::Draw(TextRenderer& Renderer) {
	for (auto& option : this->Options) {
		option.Draw(Renderer);
	}
}

void GameMenu::loadMenuFromFile(const char* gameMenuFile, unsigned int windowWidth, unsigned int windowHeight) {
	std::ifstream fileStream(gameMenuFile);
	std::string line;
	float fontSize = 12.0f;
	int offset = 0;

	while (std::getline(fileStream, line)) {
		if (line[0] != ' ') {
			this->Options.push_back(Option(line, glm::vec2(windowWidth / 2.0f - line.size() * fontSize, windowHeight / 2.0f + offset)));
			offset += 20;
		}
	}
}

void GameMenu::Load(const char* gameMenuFile, unsigned int windowWidth, unsigned int windowHeight) {
	this->loadMenuFromFile(gameMenuFile, windowWidth, windowHeight);
	if (this->Options.size() > 0) {
		this->Options[0].Selected = true;
	}
}