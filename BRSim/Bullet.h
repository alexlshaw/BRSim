#pragma once

#include "glm/glm.hpp"
#include "Agent.h"
#include "Entity.h"

class Bullet : public Entity
{
private:
	float distanceTravelled;
	float maxRange;
public:
	Agent* shooter;	//No memory management required here
	float bulletSpeed;
	glm::vec2 direction;
	bool hitTarget;
	float damage;
	Bullet(glm::vec2 position, glm::vec2 direction, float range, float damage, float speed, Agent* shooter);
	void update(float frameTime);
};