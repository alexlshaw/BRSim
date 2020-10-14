#include "Mesh.h"

Mesh::Mesh()
{
	vbo = -1;
	ibo = -1;
	vao = -1;
	initialised = false;
	indexCount = 0;
}

void Mesh::Load(std::vector<Vertex> vertices, std::vector<unsigned int> indices)
{
	//if we already had data in this mesh, clear it out
	if (initialised)
	{
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ibo);
	}
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
	//position
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)0);
	glEnableVertexAttribArray(0);
	//color
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)16);
	glEnableVertexAttribArray(1);
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	indexCount = indices.size();
	initialised = true;
}

Mesh::~Mesh()
{
	if (initialised)
	{
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ibo);
	}
}

void Mesh::draw()
{
	//default draw is with triangles
	draw(GL_TRIANGLES);
}

void Mesh::draw(GLenum primitiveType)
{
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glDrawElements(primitiveType, indexCount, GL_UNSIGNED_INT, (void*)0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}