#include <GameMenu.h>

Option::Option(std::string Value, glm::vec2 Position, float FontSize, OptionValue* value)
	:Name(Value), Position(Position), FontSize(FontSize), Selected(false), Options(), ParentOption(nullptr), Value(value) {}

void Option::Draw(TextRenderer& Renderer) {
	for (int i = 0; i < this->Options.size(); ++i) {
		auto& option = this->Options[i];
		if (i == this->Selected) {
			std::string strValue = "";
			if (option->Value) {
				strValue = " - " + std::to_string(option->Value->getValue());
			}
			Renderer.RenderText("->" + option->Name + strValue , option->Position.x - strValue.size() * FontSize / 2.0f, option->Position.y, 1.0f);
			continue;
		}
		Renderer.RenderText(option->Name, option->Position.x, option->Position.y, 1.0f);
	}
}

Option* Option::ParseOption(glm::vec2 position, std::string strOption) {
	std::vector<std::string> fSplit = split(strOption, ":");
	std::string optionName = fSplit.at(0);
	OptionValue* Value = nullptr;
	if (fSplit.size() > 1) {
		std::string optionValue = fSplit.at(1);
		if (optionValue.find("COUNTER") != std::string::npos) {
			int from = stoi(optionValue.substr(optionValue.find("(") + 1, optionValue.find("(") + 1 - optionValue.find(",")));
			int to = stoi(optionValue.substr(optionValue.find(",") + 1, optionValue.find("(") + 1 - optionValue.find(")")));
			Value = new CounterValue(from, to);
		}
	}
	position.x -= optionName.size() * 12.0f;
	return new Option(optionName, position, 12.0f, Value);
}

Option::~Option() {
	delete this->Value;
}

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

Option* GameMenu::findOptionByName(std::string optionName) {
	optionName = toLower(optionName);
	return findOptionByNameR(optionName, this->Selected->Options);
}

Option* GameMenu::findOptionByNameR(std::string optionName, std::vector<Option*> Options) {
	Option* r = nullptr;
	for (auto& option : Options) {
		if (Options.size() > 0) {
			r = findOptionByNameR(optionName, option->Options);
		}
		if (toLower(option->Name) == optionName) {
			return option;
		}
		if (r != nullptr && toLower(r->Name) == optionName) {
			return r;
		}
	}

	return nullptr;
}

void GameMenu::loadMenuFromFile(unsigned int windowWidth, unsigned int windowHeight, std::vector<Option*>& Options, Option* Parent) {
	static std::ifstream fileStream(this->Path);
	static std::string line;
	float fontSize = 12.0f;
	int offset = 0;
	int offsetInc = 20;
	static int i = 0;

	if (line != "") {
		Options.push_back(Option::ParseOption(glm::vec2(windowWidth / 2.0f, windowHeight / 2.0f + offset), line.substr(i)));
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
		
		Options.push_back(Option::ParseOption(glm::vec2(windowWidth / 2.0f, windowHeight / 2.0f + offset), line.substr(i)));
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