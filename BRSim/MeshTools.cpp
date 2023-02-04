#include "MeshTools.h"

void MeshTools::addQuad(std::vector<Vertex>* vertices, std::vector<unsigned int>* indices, float x, float y, float width, float height, glm::vec4 color)
{
	unsigned int start = static_cast<unsigned int>(vertices->size());
	Vertex botLeft = Vertex(glm::vec4(x, y, 0.0f, 1.0f), color);
	Vertex botRight = Vertex(glm::vec4(x + width, y, 0.0f, 1.0f), color);
	Vertex topLeft = Vertex(glm::vec4(x, y + height, 0.0f, 1.0f), color);
	Vertex topRight = Vertex(glm::vec4(x + width, y + height, 0.0f, 1.0f), color);

	vertices->push_back(botLeft);
	vertices->push_back(botRight);
	vertices->push_back(topLeft);
	vertices->push_back(topRight);

	//first tri
	indices->push_back(start);
	indices->push_back(start + 1);
	indices->push_back(start + 2);
	//2nd tri
	indices->push_back(start + 1);
	indices->push_back(start + 3);
	indices->push_back(start + 2);
}

void MeshTools::addQuad(std::vector<TVertex>* vertices, std::vector<unsigned int>* indices, float x, float y, float width, float height)
{
	unsigned int start = static_cast<unsigned int>(vertices->size());
	TVertex botLeft = TVertex(glm::vec4(x, y, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f));
	TVertex botRight = TVertex(glm::vec4(x + width, y, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f));
	TVertex topLeft = TVertex(glm::vec4(x, y + height, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f));
	TVertex topRight = TVertex(glm::vec4(x + width, y + height, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f));

	vertices->push_back(botLeft);
	vertices->push_back(botRight);
	vertices->push_back(topLeft);
	vertices->push_back(topRight);
	
	//first tri
	indices->push_back(start);
	indices->push_back(start + 1);
	indices->push_back(start + 2);
	//2nd tri
	indices->push_back(start + 1);
	indices->push_back(start + 3);
	indices->push_back(start + 2);
}