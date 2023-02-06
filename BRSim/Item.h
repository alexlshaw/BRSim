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

//Item classifications for the AI's convenience
enum class ItemType
{
	healthpack,
	bodyArmour,
	gunPistol,
	gunAssaultRifle,
	gunSniperRifle
};

class Item
{
public:
	ItemType itemType;
	Texture pickupTexture;
	Item(std::string textureName, ItemType itemType);
	virtual void onPickup(Agent& agent) = 0;
};