#pragma once

#include "glm/glm.hpp"
#include "Entity.h"

//Something an agent can pick up in the game
//For now, just health packs that heal on pickup

class Agent;

const float ITEM_COLLISION_RADIUS = 4.0f;

class Item : public Entity
{
public:
	Item(glm::vec2 position);
	virtual void onPickup(Agent& agent);
};