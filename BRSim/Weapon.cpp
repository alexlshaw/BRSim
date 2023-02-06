#include "Weapon.h"
#include "Agent.h"

Weapon::Weapon(std::string textureName, ItemType weaponType, EquippedWeapon awardedWeapon)
	:Item(textureName, weaponType),
	awardedWeapon(awardedWeapon) {}

void Weapon::onPickup(Agent& agent)
{
	agent.currentWeapon = awardedWeapon;
}