#include "Item.h"
#include "Agent.h"

Item::Item(glm::vec2 position)
	:Entity(position) {}

void Item::onPickup(Agent& agent)
{
	agent.currentHealth = glm::min<int>(agent.currentHealth + 80, 100);
	enabled = false;
}