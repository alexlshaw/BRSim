#pragma once

#include "glm\glm.hpp"
#include <stdio.h>

//Base class for anything present in the level that is interactable
//e.g. Agents, items

class Entity
{
public:
	glm::vec2 position;
	bool enabled;
	Entity();
	Entity(glm::vec2 position);
};