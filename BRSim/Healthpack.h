#pragma once

#include "Item.h"
#include <string>

const float HEALTHPACK_HEAL_AMOUNT = 80.0f;

class Agent;	//forward dec

class Healthpack : public Item
{
private:
public:
	Healthpack(std::string textureName);
	void onPickup(Agent& agent) override;
};