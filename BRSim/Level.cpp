#include "Level.h"

Level::Level()
	:width(2048),
	height(2048),
	tex(Texture()),
	walkTex(Texture()),
	name("level") {}

Level::Level(int width, int height, std::string levelName)
	:width(width),
	height(height),
	tex(Texture()),
	walkTex(Texture()),
	name(levelName)
{
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

LevelData Level::getLevelInfo(glm::vec2 location) const
{
	//currently we only track a binary walkable/non-walkable
	LevelData data;
	int loc = 4 * ((int)location.x + (width * (int)location.y));
	unsigned char r = levelData[loc];
	unsigned char g = levelData[loc + 1];
	unsigned char b = levelData[loc + 2];
	unsigned char a = levelData[loc + 3];
	data.walkable = (a != 0);
	return data;
}

void Level::loadLevelImages()
{
	//I wanted to load the level image data as a grid of smaller images, so that changes to parts of the level only affected a single image (for source control purposes)
	//Probably worth double-checking how well git handles small image changes to see if this is even worth it
	//There are some technical challenges with this, not to mention an overhead in code complexity, so for now more trouble than it's worth
	char fileName[128];
	sprintf_s(fileName, "Data/Level/%s/level.png", name.c_str());
	tex.loadFromPNG(fileName);
}

void Level::loadWalkData()
{
	std::vector<unsigned char> image;
	unsigned int width, height;
	char fileName[128];
	sprintf_s(fileName, "Data/Level/%s/walk.png", name.c_str());
	walkTex.loadFromPNG(fileName);
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

bool Level::locationInBounds(glm::vec2 location) const
{
	return location.x >= 0.0f && location.x < (float)width &&
		location.y >= 0.0f && location.y < (float)height;
}

glm::vec2 Level::randomWalkableLocation() const
{
	bool valid = false;
	glm::vec2 pos(0.0f, 0.0f);
	while (!valid)
	{
		pos.x = (float)(rand() % width);
		pos.y = (float)(rand() % height);
		valid = getLevelInfo(pos).walkable;
	}
	return pos;
}