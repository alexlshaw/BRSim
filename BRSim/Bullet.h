#pragma once

#include "glm/glm.hpp"
#include "Entity.h"

class Bullet : public Entity
{
private:
	float distanceTravelled;
	float maxRange;
public:
	float bulletSpeed;
	glm::vec2 direction;
	bool hitTarget;
	int ownerID;
	float damage;
	Bullet(glm::vec2 position, glm::vec2 direction, int ownerID, float range, float damage, float speed);
	void update(float frameTime);
};