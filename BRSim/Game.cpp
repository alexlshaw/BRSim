#include "Game.h"

Game::Game(Level& level)
	: levelData(level),
	circleNumber(1),
	circleDamageTick(1.0f),
	circleCentre(glm::vec2(level.width / 2.0f, level.height / 2.0f)),
	circleRadius((float)glm::max<int>(level.width, level.height) * 1.5f),
	previousCircleRadius(circleRadius),
	previousCircleCentre(circleCentre),
	elapsedShrinkTime(0.0f)
{
	initialiseItems();
	spawnItems();
	newCircle();
}

void Game::update(float deltaTime)
{
	elapsedShrinkTime += deltaTime;
	float t = elapsedShrinkTime / CIRCLE_SHRINK_TIME;
	//if we're finished shrinking this circle
	if (t >= 1.0f)
	{
		circleRadius = nextCircleRadius;
		circleCentre = nextCircleCentre;
		previousCircleRadius = circleRadius;
		previousCircleCentre = circleCentre;
		newCircle();
	}
	else
	{
		circleRadius = glm::lerp(previousCircleRadius, nextCircleRadius, t);
		circleCentre = glm::lerp(previousCircleCentre, nextCircleCentre, t);
	}
}

void Game::newCircle()
{
	nextCircleRadius = glm::max<float>(circleRadius / 2.0f, 1.0f);
	//generate locations for the new circle until we find one within the level bounds
	bool validNewCircle = false;
	while (!validNewCircle)
	{
		//compute random bearing and distance from current circle centre
		float angle = glm::radians((float)(rand() % 360));
		float distance = (float)(rand() % (int)nextCircleRadius);
		nextCircleCentre = circleCentre + glm::vec2(glm::cos(angle) * distance, glm::sin(angle) * distance);
		if (levelData.locationInBounds(nextCircleCentre))
		{
			validNewCircle = true;
		}
	}
	elapsedShrinkTime = 0.0f;
	circleNumber++;
	circleDamageTick *= 2.0f;
}

bool Game::isPositionInsideNextCircle(glm::vec2 position) const
{
	float distanceToCentre = glm::length(position - nextCircleCentre);
	return distanceToCentre < nextCircleRadius;

}

bool Game::isPositionInsideCurrentCircle(glm::vec2 position) const
{
	float distanceToCentre = glm::length(position - circleCentre);
	return distanceToCentre < circleRadius;

}

void Game::initialiseItems()
{
	EquippedWeapon pistol(WeaponType::pistol, 20.0f, 1.0f, 60.0f, 100.0f);
	EquippedWeapon assaultRifle(WeaponType::assaultRifle, 20.0f, 0.35f, 60.0f, 100.0f);
	EquippedWeapon sniperRifle(WeaponType::sniperRifle, 50.0f, 2.0f, 180.0f, 150.0f);

	healthpackBase = std::make_unique<Healthpack>("Healthpack.png");
	bodyArmourBase = std::make_unique<BodyArmour>("BodyArmour.png");
	pistolBase = std::make_unique<Weapon>("Gun-Pistol.png", ItemType::gunPistol, pistol);
	assaultRifleBase = std::make_unique<Weapon>("Gun-AssaultRifle.png", ItemType::gunAssaultRifle, assaultRifle);
	sniperRifleBase = std::make_unique<Weapon>("Gun-SniperRifle.png", ItemType::gunSniperRifle, sniperRifle);
}

void Game::spawnItems()
{
	items.reserve(TOTAL_ITEM_SPAWNS);
	spawnItemSet(*healthpackBase, items, HEALTHPACK_COUNT);
	spawnItemSet(*bodyArmourBase, items, ARMOUR_COUNT);
	spawnItemSet(*pistolBase, items, PISTOL_COUNT);
	spawnItemSet(*assaultRifleBase, items, ASSAULT_RIFLE_COUNT);
	spawnItemSet(*sniperRifleBase, items, SNIPER_RIFLE_COUNT);
}

void Game::spawnItemSet(Item& item, std::vector<ItemInstance>& items, int numberToSpawn)
{
	for (int i = 0; i < numberToSpawn; i++)
	{
		items.push_back(ItemInstance(item, levelData.randomWalkableLocation()));
	}
}

void Game::restart()
{
	//reset the circles
	elapsedShrinkTime = 0.0f;
	circleNumber = 1;
	circleDamageTick = 1.0f;
	circleCentre = glm::vec2(levelData.width / 2.0f, levelData.height / 2.0f);
	circleRadius = (float)glm::max<int>(levelData.width, levelData.height) * 1.5f;
	previousCircleRadius = circleRadius;
	previousCircleCentre = circleCentre;
	newCircle();
	//reset the items
	items.clear();
	spawnItems();
}