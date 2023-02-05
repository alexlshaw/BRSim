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
	~Level();
	int width, height;
	std::string name;
	LevelData getLevelInfo(float x, float y);
	Texture* tex;
	Texture* walkTex;
	bool locationInBounds(glm::vec2 location);
private:
	std::vector<unsigned char> levelData;
	void loadLevelImages();
	void loadWalkData();
};