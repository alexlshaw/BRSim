#include "Mesh.h"

Mesh::Mesh()
	:vbo(-1),
	ibo(-1),
	vao(-1),
	initialised(false),
	indexCount(0) {}


void Mesh::Load(std::vector<Vertex> vertices, std::vector<unsigned int> indices)
{
	initialiseMesh();
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
	Vertex::setAttribs();
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	indexCount = (GLsizei)indices.size();
	
}

void Mesh::Load(std::vector<TVertex> vertices, std::vector<unsigned int> indices)
{
	initialiseMesh();
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TVertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
	TVertex::setAttribs();
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	indexCount = (GLsizei)indices.size();
}

void Mesh::initialiseMesh()
{
	//if we already had data in this mesh, clear it out
	if (initialised)
	{
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ibo);
		//TODO: Check whether I also need to delete the array (or can avoid regenerating the array, etc)
	}
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
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