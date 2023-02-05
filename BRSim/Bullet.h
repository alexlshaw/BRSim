#pragma once

#include "glm/glm.hpp"
#include "Entity.h"

const float BULLET_BASE_DAMAGE = 20.0f;
const float BULLET_SPEED = 100.0f;

class Bullet : public Entity
{
private:
	float life;
public:
	glm::vec2 direction;
	bool hitTarget;
	int ownerID;
	float damage;
	Bullet(glm::vec2 position, glm::vec2 direction, float lifespan, int owner);
	void update(float frameTime);
};