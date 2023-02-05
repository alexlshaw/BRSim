#pragma once

#include "glm/glm.hpp"
#include "Entity.h"

const int BULLET_BASE_DAMAGE = 20;

class Bullet : public Entity
{
public:
	Bullet(glm::vec2 position, glm::vec2 direction, float lifespan, int owner);
	glm::vec2 dir;
	float life;
	bool hitTarget;
	int ownerID;
	int damage;
private:
};