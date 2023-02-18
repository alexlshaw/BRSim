#include "Bullet.h"

Bullet::Bullet(glm::vec2 position, glm::vec2 direction, float range, float damage, float speed, Agent* shooter)
	:Entity(position),
	direction(direction),
	maxRange(range),
	damage(damage),
	bulletSpeed(speed),
	hitTarget(false),
	distanceTravelled(0.0f),
	shooter(shooter)
{}

void Bullet::update(float frameTime)
{
	float distanceThisFrame = frameTime * bulletSpeed;
	position += direction * distanceThisFrame;
	distanceTravelled += distanceThisFrame;
	if (hitTarget || distanceTravelled >= maxRange)
	{
		enabled = false;
	}
}