#pragma once

#include "glm/glm.hpp"

class Bullet
{
public:
	Bullet(glm::vec2 position, glm::vec2 direction, float lifespan, int owner);
	glm::vec2 pos;
	glm::vec2 dir;
	float life;
	bool hitTarget;
	int ownerID;
private:
};