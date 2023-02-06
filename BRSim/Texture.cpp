#include "Texture.h"

Texture::Texture()
	:textureIndex(-1) {}

Texture::Texture(const char* fileName)
{
	loadFromPNG(fileName);
}

Texture::~Texture()
{
	glDeleteTextures(1, &textureIndex);
}

void Texture::use() const
{
	glBindTexture(GL_TEXTURE_2D, textureIndex);
}

void Texture::loadFromPixels(std::vector<GLubyte> pixels, int width, int height)
{
	glGenTextures(1, &textureIndex);
	glBindTexture(GL_TEXTURE_2D, textureIndex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)&pixels[0]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::loadFromPNG(const char* fileName)
{
	std::vector<unsigned char> image;
	unsigned int width, height;
	unsigned int error = lodepng::decode(image, width, height, fileName);

	//vertically flip the image
	std::vector<unsigned char> flippedImage;
	flippedImage.resize(image.size());
	//lodepng loads image data vertically flipped wrt openGL ordering so flip it
	for (unsigned int x = 0; x < width * 4; x++)
	{
		for (unsigned int y = 0; y < height; y++)
		{
			unsigned int inputLoc = x + (width * y * 4);
			unsigned int outputLoc = x + (width * (height - y - 1) * 4);
			flippedImage[outputLoc] = image[inputLoc];
		}
	}


	if (error != 0)
	{
		printf("Error loading texture: %s\n", fileName);
	}
	loadFromPixels(flippedImage, width, height);
}