#include "Bullet.h"

Bullet::Bullet(glm::vec2 position, glm::vec2 direction, float lifespan, int owner)
	:Entity(position)
{
	dir = direction;
	life = lifespan;
	hitTarget = false;
	ownerID = owner;
	damage = BULLET_BASE_DAMAGE;
}