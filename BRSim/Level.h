#pragma once

#include <string>

#include "glm\glm.hpp"
#include "Texture.h"

const bool LOAD_LEVEL_SURFACE_DATA = true;

class LevelData
{
public:
	bool walkable;
};

class Level
{
public:
	Level();
	Level(int width, int height, std::string levelName);
	int width, height;
	std::string name;
	Texture tex;
	Texture walkTex;
	LevelData getLevelInfo(glm::vec2 location) const;
	bool locationInBounds(glm::vec2 location) const;
	glm::vec2 randomWalkableLocation() const;
private:
	std::vector<unsigned char> levelData;
	void loadLevelImages();
	void loadWalkData();
};