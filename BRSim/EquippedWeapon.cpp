#include "EquippedWeapon.h"

EquippedWeapon::EquippedWeapon()
	:weaponType(WeaponType::none),
	bulletDamage(0.0f),
	timeBetweenShots(0.0f),
	range(0.0f),
	bulletSpeed(0.0f) {}

EquippedWeapon::EquippedWeapon(WeaponType weaponType, float bulletDamage, float timeBetweenShots, float range, float bulletSpeed)
	:weaponType(weaponType),
	bulletDamage(bulletDamage),
	timeBetweenShots(timeBetweenShots),
	range(range),
	bulletSpeed(bulletSpeed)
{}