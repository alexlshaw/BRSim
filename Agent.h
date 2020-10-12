#pragma once

#include "glm/glm.hpp"
#include "Settings.h"

class Agent
{
public:
	Agent(glm::vec2 position, float direction);
	~Agent();
	glm::vec2 pos;
	float look;	//bearing of the agent's look direction (in radians)
	float range;
	bool alive;
	glm::vec2 forward();
private:
	
};