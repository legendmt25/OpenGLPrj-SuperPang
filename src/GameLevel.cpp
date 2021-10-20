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
			bool solid;

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

			sstream >> solid;
			vData.push_back(Data(type, position, size, solid));
		}
		if(vData.size() > 0) {
			Init(vData, width, height);
		}
	}
}

void GameLevel::Init(std::vector<Data> vData, unsigned int levelWidth, unsigned int levelHeight) {
	for (auto& data : vData) {
		if (data.type == "BALL") {
			this->Objects.push_back(new BallObject(data.pos, data.size.x / 2.0f, ResourceManager::GetTexture("ball"), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(130.0f, 190.0f, 0.0f)));
		}
		if (data.type == "BLOCK") {
			GameObject* obj = new PowerUpObject(data.pos, data.size, ResourceManager::GetTexture("block"), glm::vec3(1.0f), glm::vec3(0.0f));
			if (data.solid) {
				obj->IsSolid = true;
				//obj->Texture = ResourceManager::GetTexture("block-solid");
			}
			this->Objects.push_back(obj);
		}
	}
}

bool GameLevel::isCompleted() {
	for (auto& object : this->Objects) {
		if (!object->IsSolid && !object->Destroyed) {
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
}

void GameLevel::Reset() {
	for (auto& object : this->Objects) {
		delete object;
	}
	this->Load(ww, wh);
}