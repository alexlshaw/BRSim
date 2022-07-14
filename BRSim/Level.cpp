#include "Level.h"

Level::Level() 
{
	width = 2048;
	height = 2048;
	tex = new Texture();
	walkTex = new Texture();
	name = "level";
}

Level::Level(int width, int height, string levelName)
{
	this->width = width;
	this->height = height;
	tex = new Texture();
	walkTex = new Texture();
	name = levelName;
	buildMesh();
	loadLevelImages();
	printf("Level Texture loaded\n");
	if (LOAD_LEVEL_SURFACE_DATA)
	{
		loadWalkData();
		printf("Level walkability loaded\n");
	}
	else
	{
		printf("Skipping level walk data\n");
	}
}

Level::~Level() 
{
	delete tex;
	delete walkTex;
}

void Level::draw(Shader* shader, GLuint uModel, bool walkLayer)
{
	glm::vec3 offset = glm::vec3(0.0f, 0.0f, 0.0f);// glm::vec3(x * (float)tileSize, y * (float)tileSize, 0.0f);
	glm::mat4 tr = glm::translate(offset);
	//set the shader's uniform
	shader->setUniform(uModel, tr);
	if (walkLayer)
	{
		walkTex->use();
	}
	else
	{
		tex->use();
	}
	mesh.draw();
}

LevelData Level::getLevelInfo(float x, float y)
{
	//So there is some interesting behaviour here:
	//At the moment, calling this function seems to only find 0 0 0 0 pixels
	//Calling this function wth values that I'm 99% sure should be ok (1024, 1024)
	//still finds a 0 0 0 0 pixel val
	//it shouldn't be a image-flipping issue, since 1024 1024 should be black (0, 0, 0, 255) no matter what orientation the image holds
	//looks like it's just fucky indexing into the array


	//currently we only track a binary walkable/non-walkable
	LevelData data;
	int loc = 4 * ((int)x + (width * (int)y));
	unsigned char r = levelData[loc];
	unsigned char g = levelData[loc + 1];
	unsigned char b = levelData[loc + 2];
	unsigned char a = levelData[loc + 3];
	data.walkable = (a != 0);
	return data;
}

void Level::buildMesh()
{
	std::vector<TVertex> vertices;
	std::vector<unsigned int> indices;
	//mesh is a simple 2-triangle quad
	TVertex bl = TVertex(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f));
	TVertex br = TVertex(glm::vec4((float)width, 0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f));
	TVertex tl = TVertex(glm::vec4(0.0f, (float)height, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f));
	TVertex tr = TVertex(glm::vec4((float)width, (float)height, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f));
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
	//I wanted to load the level image data as a grid of smaller images, so that changes to parts of the level only affected a single image
	//Probably worth double-checking how well git handles small image changes to see if this is even worth it
	//There are some technical challenges with this, not to mention an overhead in code complexity, so for now more trouble than it's worth
	char fileName[128];
	sprintf_s(fileName, "Data/Level/%s/level.png", name.c_str());
	tex->loadFromPNG(fileName);
}

void Level::loadWalkData()
{
	std::vector<unsigned char> image;
	unsigned int width, height;
	char fileName[128];
	sprintf_s(fileName, "Data/Level/%s/walk.png", name.c_str());
	walkTex->loadFromPNG(fileName);
	unsigned int error = lodepng::decode(image, width, height, fileName);
	if (error != 0)
	{
		printf("Error loading walk file: %s\n", fileName);
	}
	//vertically flip the image
	levelData.resize(image.size());
	//lodepng loads image data vertically flipped wrt openGL ordering so flip it
	for (unsigned int x = 0; x < width * 4; x++)
	{
		for (unsigned int y = 0; y < height; y++)
		{
			unsigned int inputLoc = x + (width * y * 4);
			unsigned int outputLoc = x + (width * (height - y - 1) * 4);
			levelData[outputLoc] = image[inputLoc];
		}
	}
}