#pragma once

#include <string>

#include "lodepng.h"
#include "Settings.h"
#include "Texture.h"

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
private:
	std::vector<unsigned char> levelData;
	void loadLevelImages();
	void loadWalkData();
};