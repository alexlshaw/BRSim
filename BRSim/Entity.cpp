#include "Entity.h"

Entity::Entity()
	:position(glm::vec2(0.0f, 0.0f)),
	enabled(true) {}

Entity::Entity(glm::vec2 position)
	:position(position),
	enabled(true) {}