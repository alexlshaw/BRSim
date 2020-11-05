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

class Level
{
public:
	Level();
	Level(int width, int height, int tileSize);
	~Level();
	float levelWidth;
	float levelHeight;
	int tileSize, width, height;
	void draw(Shader* shader, GLuint uModel);
private:
	Mesh mesh;
	Texture* tex;
	std::vector<unsigned char> levelImageData;
	void buildMesh();
	void loadLevelImages();
};