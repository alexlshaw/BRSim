#include "Bullet.h"

Bullet::Bullet(glm::vec2 position, glm::vec2 direction, float lifespan, int owner)
	:Entity(position),
	direction(direction),
	life(lifespan),
	hitTarget(false),
	ownerID(owner),
	damage(BULLET_BASE_DAMAGE)
{}

void Bullet::update(float frameTime)
{
	life -= frameTime;
	position += direction * frameTime * BULLET_SPEED;
	if (hitTarget || life <= 0.0f)
	{
		enabled = false;
	}
}