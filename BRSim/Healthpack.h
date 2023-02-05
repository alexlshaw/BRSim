#pragma once

#include "Item.h"
#include "Texture.h"
#include <stdio.h>
#include <string>

const int HEALTHPACK_HEAL_AMOUNT = 80;

class Agent;

class Healthpack : public Item
{
private:
public:
	Healthpack(std::string textureName);
	void onPickup(Agent& agent) override;
};