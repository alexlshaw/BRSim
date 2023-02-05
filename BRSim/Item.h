#pragma once

#include "glm/glm.hpp"
#include <string>

#include "Texture.h"

//Something an agent can pick up in the game
//Represents the base object (and rules of interaction), rather than the thing that is placed in the world
//For the in-world entity, see ItemInstance
//Objects of this class' child classes should in theory be singletons, but we're not enforcing it in case it's later desirable to have variations

class Agent;

const float ITEM_COLLISION_RADIUS = 4.0f;

class Item
{
private:
	Texture pickupTexture;
public:
	Item(std::string textureName);
	virtual void onPickup(Agent& agent) = 0;
};