#pragma once

#include "glm\glm.hpp"

class Vertex
{
public:
	glm::vec4 position;
	glm::vec4 color;
	Vertex(glm::vec4 pos, glm::vec4 col) { position = pos; color = col; }
	static void setAttribs()
	{
		//position
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)0);
		glEnableVertexAttribArray(0);
		//color
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)16);
		glEnableVertexAttribArray(1);
	}
};

class TVertex	//Textured Vertex
{
public:
	glm::vec4 position;
	glm::vec2 coord;
	TVertex(glm::vec4 pos, glm::vec2 coord) { position = pos; this->coord = coord; }
	static void setAttribs()
	{
		//position
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(TVertex), (const GLvoid*)0);
		glEnableVertexAttribArray(0);
		//texcoord
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TVertex), (const GLvoid*)16);
		glEnableVertexAttribArray(1);
	}
};