#pragma once

#include "Item.h"
#include <string>

const float BODYARMOUR_ARMOUR_AMOUNT = 50.0f;

class Agent;	//Forward dec

class BodyArmour : public Item
{
private:
public:
	BodyArmour(std::string textureName);
	void onPickup(Agent& agent) override;
};