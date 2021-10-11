#include <vector>

#include "SpriteRenderer.h"
#include "BallObject.h"

class Data {
public:
	std::string type;
	glm::vec2 pos;
	glm::vec2 size;
	bool solid;
	Data(std::string type, glm::vec2 pos, glm::vec2 size, bool solid)
		:type(type), pos(pos), size(size), solid(solid) {}
};

class GameLevel
{
public:
	char* file;
	std::vector<GameObject*> Objects;

	GameLevel(const char* file);
	~GameLevel();
	void Load(unsigned int windowWidth, unsigned int windowHeight);
	void Draw(SpriteRenderer& renderer);
	bool isCompleted();
	void Reset();
public:
	void Init(std::vector<Data> Data, unsigned int levelWidth, unsigned int levelHeight);
};

