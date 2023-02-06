#pragma once

#include "EquippedWeapon.h"
#include "Item.h"
#include <string>

class Agent;

class Weapon : public Item
{
public:
	EquippedWeapon awardedWeapon;	//The weapon given to the agent when the agent picks this up
	Weapon(std::string textureName, ItemType weaponType, EquippedWeapon awardedWeapon);
	void onPickup(Agent& agent) override;
};