#pragma once

#include "Entity.h"
#include "glm\glm.hpp"
#include "Item.h"

//The interactable item placed in the world

class Agent;

class ItemInstance : public Entity
{
private:
	Item& baseItem;
public:
	ItemInstance(Item& baseItem, glm::vec2 position);
	void onPickup(Agent& agent);
};