#pragma once

#include "glm/glm.hpp"
#include <vector>

#include "Vertex.h"

namespace MeshTools
{
	void addQuad(std::vector<Vertex>* vertices, std::vector<unsigned int>* indices, float x, float y, float width, float height, glm::vec4 color);

	void addQuad(std::vector<TVertex>* vertices, std::vector<unsigned int>* indices, float x, float y, float width, float height);
}