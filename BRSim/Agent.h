#pragma once

#include "glm/glm.hpp"
#include "Settings.h"

class Agent
{
public:
	Agent(glm::vec2 position, float direction, int identity);
	~Agent();
	glm::vec2 pos;
	float look;	//bearing of the agent's look direction (in radians)
	float range;
	float shotCooldownRemainingTime;
	bool alive;
	int id;
	glm::vec2 forward();
	bool hasTarget;
	glm::vec2 targetPosition;	//Location the agent wants to move to unless it is in a fight
	bool rotateTowards(glm::vec2 target, float deltaTime);	//returns true if rotation is enough to face target
	bool moveTowards(glm::vec2 target, float deltaTime);	//returns true if movement is enough to reach target
	void setTarget(glm::vec2 target);
private:
	
};