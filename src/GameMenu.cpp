#include <GameMenu.h>

Option::Option(std::string Value, glm::vec2 Position, float FontSize, std::list<Option> Options)
	:Value(Value), Position(Position), FontSize(FontSize), Selected(false) {}

void Option::AlignCenter(unsigned int Width, unsigned int Height, float HeightOffset) {
	float fontSize = 12.0f;
	this->Position = glm::vec2((Width - Value.size() * fontSize) / 2.0f, Height / 2.0f + HeightOffset);
}

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

void GameMenu::loadMenuFromFile(const char* gameMenuFile) {
	std::ifstream fileStream(gameMenuFile);
	std::string line;
	while (std::getline(fileStream, line)) {
		std::stringstream stringStream(line);

	}
}