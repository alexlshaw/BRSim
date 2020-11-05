#include "Level.h"

Level::Level() 
{
	levelWidth = 4000.0f;
	levelHeight = 4000.0f;
	width = 1;
	height = 1;
	tileSize = 4000;
	tex = new Texture[1];
}

Level::Level(int width, int height, int tileSize)
{
	this->width = width;
	this->height = height;
	this->tileSize = tileSize;
	levelWidth = (float)(width * tileSize);
	levelHeight = (float)(height * tileSize);
	tex = new Texture[width * height];
	buildMesh();
	loadLevelImages();
}

Level::~Level() 
{
	delete[] tex;
}

void Level::draw(Shader* shader, GLuint uModel)
{
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			glm::vec3 offset = glm::vec3(x * (float)tileSize, y * (float)tileSize, 0.0f);
			glm::mat4 tr = glm::translate(offset);
			//set the shader's uniform
			shader->setUniform(uModel, tr);
			tex[x + width * y].use();
			mesh.draw();
		}
	}
}

void Level::buildMesh()
{
	std::vector<TVertex> vertices;
	std::vector<unsigned int> indices;
	//mesh is a simple 2-triangle quad
	TVertex bl = TVertex(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f));
	TVertex br = TVertex(glm::vec4((float)tileSize, 0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f));
	TVertex tl = TVertex(glm::vec4(0.0f, (float)tileSize, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f));
	TVertex tr = TVertex(glm::vec4((float)tileSize, (float)tileSize, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f));
	vertices.push_back(bl);
	vertices.push_back(br);
	vertices.push_back(tl);
	vertices.push_back(tr);
	//first tri
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);
	//2nd tri
	indices.push_back(1);
	indices.push_back(3);
	indices.push_back(2);

	mesh.Load(vertices, indices);
}

void Level::loadLevelImages()
{
	//so, the issue here is that the image as a whole is not laid out in memory in these
	//convenient blocks
	//rather, it's more like: bottom row of pixels for bottom images
	//then 2nd row of pixels for bottom images, etc
	
	
	//1. Allocate enough memory for a levelWidth * levelHeight array of image data
	//2. for each image, determine index into this block that should rep the image
	//3. load the image into this buffer, at the target index
	//4. Dump the whole buffer on to the GPU as a texture

	//So, the issue is that each image isn't actually a contiguous block in memory when being
	//laid out like this, so instead I think it needs to be done row by row

	//So for now, let's just use separate textures
	char fileName[128];
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < width; y++)
		{
			sprintf_s(fileName, "Data/Level/level_%i_%i.png", x, y);
			tex[x + (width * y)].loadFromPNG(fileName);
		}
	}
}