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
	void draw(Shader* shader, GLuint uModel, bool walkLayer);
	LevelData getLevelInfo(float x, float y);
private:
	Mesh mesh;
	Texture* tex;
	Texture* walkTex;
	std::vector<unsigned char> levelData;
	void buildMesh();
	void loadLevelImages();
	void loadWalkData();
};