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

Option::~Option() = default;

int countSpaces(std::string line) {
	int spaces = 0;
	while (line[spaces] == ' ') {
		++spaces;
	}
	return spaces;
}


GameMenu::GameMenu(std::string path) 
	: Selected(new Option("Press Enter", glm::vec2(0.0f))), Path(path) {}

void GameMenu::Draw(TextRenderer& Renderer) {
	this->Selected->Draw(Renderer);
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

void GameMenu::DestroyRecursive(std::vector<Option*>& Options) {
	for (auto& option : Options) {
		if (option->Options.size() > 0) {
			this->DestroyRecursive(option->Options);
		}
		option->Options.clear();
		delete option;
	}
	Options.clear();
}

GameMenu::~GameMenu() {
	while (this->Selected->ParentOption != nullptr) {
		this->Selected = this->Selected->ParentOption;
	}
	this->DestroyRecursive(this->Selected->Options);
	delete this->Selected;
}