#pragma once

#include "glm/glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtx\transform.hpp" 
#include "lodepng.h"
#include "Mesh.h"
#include "Settings.h"
#include "Shader.h"
#include "Texture.h"
#include "Vertex.h"
#include <string>

class LevelData
{
public:
	bool walkable;
};

class Level
{
public:
	Level();
	Level(int width, int height, string levelName);
	~Level();
	int width, height;
	string name;
	LevelData getLevelInfo(float x, float y);
	Texture* tex;
	Texture* walkTex;
private:
	std::vector<unsigned char> levelData;
	void loadLevelImages();
	void loadWalkData();
};