#pragma once

#include "glm/glm.hpp"

//Something an agent can pick up in the game
//For now, just health packs that heal on pickup

class Agent;

const float ITEM_COLLISION_RADIUS = 4.0f;

class Item
{
public:
	bool available;		//Rather than deleting items when they're picked up, we just flag them as gone
	Item(glm::vec2 location);
	glm::vec2 location;
	virtual void onPickup(Agent& agent);
};