#include "Item.h"
#include "Agent.h"

Item::Item(glm::vec2 location)
	:location(location)
{
	available = true;
}

void Item::onPickup(Agent& agent)
{
	agent.currentHealth = glm::min<int>(agent.currentHealth + 80, 100);
	available = false;
}