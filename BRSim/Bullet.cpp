#include "Bullet.h"

Bullet::Bullet(glm::vec2 position, glm::vec2 direction, int ownerID, float range, float damage, float speed)
	:Entity(position),
	direction(direction),
	ownerID(ownerID),
	maxRange(range),
	damage(damage),
	bulletSpeed(speed),
	hitTarget(false),
	distanceTravelled(0.0f)
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