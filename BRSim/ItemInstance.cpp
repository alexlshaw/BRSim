#include "ItemInstance.h"
#include "Agent.h"

ItemInstance::ItemInstance(Item& baseItem, glm::vec2 position)
	:baseItem(baseItem), Entity(position) {}

void ItemInstance::onPickup(Agent& agent)
{
	baseItem.onPickup(agent);
	enabled = false;
}