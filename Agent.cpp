#include "Agent.h"

Agent::Agent(glm::vec2 position, float direction)
{
	look = direction;
	pos = position;
	range = AGENT_DEFAULT_RANGE;
	alive = true;
}

Agent::~Agent() {}

glm::vec2 Agent::forward()
{
	//this should be normalized by how it is computed
	return glm::vec2(glm::cos(look), glm::sin(look));
}