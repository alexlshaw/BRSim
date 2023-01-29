#pragma once

#include "glad/glad.h"
#include <cstdlib>
#include <vector>
#include "lodepng.h"
#pragma

class Texture
{
private:
	GLuint textureIndex;
public:
	Texture();
	Texture(const char* filename);
	~Texture();
	void loadFromPixels(std::vector<GLubyte> pixels, int width, int height);	//4 elements in array define a pixel in rgba format
	void loadFromPNG(const char* fileName);
	void use();
};