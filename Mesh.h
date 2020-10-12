#pragma once

#include <vector>
#include "glad/glad.h"
#include "glm/glm.hpp"
#include "Vertex.h"

class Mesh
{
public:
	Mesh();
	void Load(std::vector<Vertex> vertices, std::vector<unsigned int> indices);
	~Mesh();
	void draw();
	void draw(GLenum primitiveType);
private:
	GLuint vao, vbo, ibo;
	bool initialised;
	unsigned int indexCount;
};