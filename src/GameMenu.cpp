#include <GameMenu.h>
#include <iostream>

Option::Option(std::string Value, glm::vec2 Position, float FontSize)
	:Value(Value), Position(Position), FontSize(FontSize), Selected(false), Options(), ParentOption(nullptr) {}

void Option::Draw(TextRenderer& Renderer) {
	for (int i = 0; i < this->Options.size(); ++i) {
		auto& option = this->Options[i];
		if (i == this->Selected) {
			Renderer.RenderText("->" + option->Value, option->Position.x, option->Position.y, 1.0f);
			continue;
		}
		Renderer.RenderText(option->Value, option->Position.x, option->Position.y, 1.0f);
	}
}

void GameMenu::Draw(TextRenderer& Renderer) {
	this->Selected->Draw(Renderer);
}

int countSpaces(std::string line) {
	int spaces = 0;
	while (line[spaces] == ' ') {
		++spaces;
	}
	return spaces;
}

void GameMenu::loadMenuFromFile(unsigned int windowWidth, unsigned int windowHeight, std::vector<Option*>& Options, Option* Parent) {
	static std::ifstream fileStream(this->Path);
	static std::string line;
	float fontSize = 12.0f;
	int offset = 0;
	int offsetInc = 20;
	static int i = 0;

	if (line != "") {
		Options.push_back(new Option(line.substr(i), glm::vec2(windowWidth / 2.0f - line.size() * fontSize, windowHeight / 2.0f + offset)));
		Options[Options.size() - 1]->ParentOption = Parent;
		offset += offsetInc;
	}

	while (std::getline(fileStream, line)) {
		if (countSpaces(line) > i) {
			++i;
			this->loadMenuFromFile(windowWidth, windowHeight, Options[Options.size() - 1]->Options, Options[Options.size() - 1]);
		}

		if (countSpaces(line) < i) {
			--i;
			return;
		}

		Options.push_back(new Option(line.substr(i), glm::vec2(windowWidth / 2.0f - line.size() * fontSize, windowHeight / 2.0f + offset)));
		Options[Options.size() - 1]->ParentOption = Parent;
		offset += offsetInc;
	}
}

void GameMenu::Load(unsigned int windowWidth, unsigned int windowHeight) {
	this->loadMenuFromFile(windowWidth, windowHeight, this->Selected->Options, this->Selected);
}