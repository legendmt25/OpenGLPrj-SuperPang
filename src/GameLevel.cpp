#include "GameLevel.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>

#include "ResourceManager.h"

GameLevel::GameLevel(const char* file) {
	this->file = new char[strlen(file) + 1];
	strcpy(this->file, file);
}

GameLevel::~GameLevel() {
	delete[] this->file;
	for (auto& object : this->Objects) {
		delete object;
	}

	for (auto& object : this->PowerUps) {
		delete object;
	}
	this->Objects.clear();
	this->Objects.clear();
}

unsigned int ww, wh;

void GameLevel::Load(unsigned int windowWidth, unsigned int windowHeight) {
	ww = windowWidth;
	wh = windowHeight;

	this->Objects.clear();
	std::string line;
	std::ifstream fstream(this->file);
	std::vector<Data> vData;

	if (fstream) {
		std::getline(fstream, line);
		std::istringstream sstream(line);
		int width, height;
		sstream >> width;
		sstream >> height;

		while (std::getline(fstream, line)) {
			sstream = std::istringstream(line);
			std::string type;
			glm::vec3 position;
			glm::vec3 size;

			sstream >> type;
			sstream >> position.x;
			sstream >> position.y;
			sstream >> size.x;
			if (type == "BALL") {
				size.x *= 2.0f;
				size.y *= 2.0f;
			}
			else {
				sstream >> size.y;
			}

			size.x = size.x / width * (float)windowWidth;
			size.y = size.y / height * (float)windowHeight;

			position.z = 0.0f;
			size.z = 1.0f;

			vData.push_back(Data(type, position, size, false));
		}
		if(vData.size() > 0) {
			Init(vData, width, height);
		}
	}
}

void GameLevel::Init(std::vector<Data> vData, unsigned int levelWidth, unsigned int levelHeight) {
	for (auto& data : vData) {
		if (data.type == "BALL") {
			this->Attackers.push_back(new BallObject(data.pos, data.size.x / 2.0f, ResourceManager::GetTexture("ball"), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(130.0f, 190.0f, 0.0f)));
		}
		if (data.type == "BLOCK") {
			this->Objects.push_back(new BlockObject(data.pos, data.size, ResourceManager::GetTexture("-block-solid"), glm::vec3(1.0f), glm::vec3(0.0f)));
			this->Objects[this->Objects.size() - 1]->IsSolid = true;
		}
		if (data.type == "BLOCKPOWERUP") {
			this->Objects.push_back(new BlockObject(data.pos, data.size, ResourceManager::GetTexture("-block"), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.0f)));
		}
		if (data.type == "HEXAGON") {
			this->Attackers.push_back(new HexagonObject(data.pos, data.size, ResourceManager::GetTexture("hexagon-1"), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(130.0f, 190.0f, 0.0f)));
		}
		if (data.type == "LADDER") {
			this->Objects.push_back(new LadderObject(data.pos, data.size));
		}
	}
}

bool GameLevel::isCompleted() {
	for (auto& object : this->Attackers) {
		if (!object->Destroyed && !object->IsSolid) {
			return false;
		}
	}
	return true;
}

void GameLevel::Draw(Sprite3DRenderer& Renderer3D) {

	for (auto& object : this->Objects) {
		if (!object->Destroyed) {
			object->Draw(Renderer3D);
		}
	}

	for (auto& object : this->PowerUps) {
		if (!object->Destroyed) {
			object->Draw(Renderer3D);
		}
	}
}

void GameLevel::Reset() {
	for (auto& object : this->Attackers) {
		delete object;
	}
	for (auto& object : this->Objects) {
		delete object;
	}
	for (auto& object : this->PowerUps) {
		delete object;
	}
	this->Objects.clear();
	this->PowerUps.clear();
	this->Attackers.clear();
	this->Load(ww, wh);
}