#include <GameMenu.h>

Option::Option(std::string Value, glm::vec2 Position, float FontSize)
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