#pragma once

enum class WeaponType
{
	none = 0,
	pistol = 1,
	assaultRifle = 2,
	sniperRifle = 3
};	//IMPORTANT: The values in this enum are used as indices for arrays in the AIWeights class

//This is the class that represents a weapon the agent currently is using
//For the item pickup that spawns in the level (and sets this), see the class Weapon
class EquippedWeapon
{
public:
	float bulletDamage;
	float timeBetweenShots;
	float range;
	float bulletSpeed;
	WeaponType weaponType;
	EquippedWeapon();	//Default constructor only used for initialising Agents. Don't ever call this one manually.
	EquippedWeapon(WeaponType weaponType, float bulletDamage, float timeBetweenShots, float range, float bulletSpeed);
};