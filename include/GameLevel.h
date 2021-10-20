#include <vector>

#include "Sprite3DRenderer.h"
#include "BallObject.h"
#include "PowerUpObject.h"
#include "BlockObject.h"

class Data {
public:
	std::string type;
	glm::vec3 pos;
	glm::vec3 size;
	bool solid;
	Data(std::string type, glm::vec3 pos, glm::vec3 size, bool solid)
		:type(type), pos(pos), size(size), solid(solid) {}
};

class GameLevel
{
public:
	char* file;
	std::vector<GameObject*> Objects;
	std::vector<PowerUpObject*> PowerUps;

	GameLevel(const char* file);
	~GameLevel();
	void Load(unsigned int windowWidth, unsigned int windowHeight);
	void Draw(Sprite3DRenderer& renderer);
	bool isCompleted();
	void Reset();
public:
	void Init(std::vector<Data> Data, unsigned int levelWidth, unsigned int levelHeight);
};

