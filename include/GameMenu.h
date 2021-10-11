#include <vector>
#include <TextRenderer.h>

class Option {
public:
	std::string Value;
	glm::vec2 Position;
	bool Selected;
	float FontSize;
	
	Option(std::string Value, glm::vec2 Position, float FontSize = 12.0f);
	void Draw(TextRenderer& Renderer);
};

class GameMenu
{
public:
	std::vector<Option> Options;
	unsigned int Selected;

	GameMenu() : Selected(0) {}
	GameMenu(std::vector<Option> Options);
	void Draw(TextRenderer& Renderer);
};