#pragma once

#include <vector>
#include <memory>
#include "glm\gtx\compatibility.hpp"
#include "ItemInstance.h"
#include "Healthpack.h"
#include "BodyArmour.h"
#include "Level.h"
#include "Weapon.h"

const int HEALTHPACK_COUNT = 20;
const int ARMOUR_COUNT = 15;
const int PISTOL_COUNT = 20;
const int ASSAULT_RIFLE_COUNT = 12;
const int SNIPER_RIFLE_COUNT = 6;
const int TOTAL_ITEM_SPAWNS = HEALTHPACK_COUNT + ARMOUR_COUNT + PISTOL_COUNT + ASSAULT_RIFLE_COUNT + SNIPER_RIFLE_COUNT;
const float CIRCLE_SHRINK_TIME = 20.0f;		//default 20

//Responsible for managing the gameplay stuff (tracking circles and other events)
class Game
{
private:
	std::unique_ptr<Healthpack> healthpackBase;
	std::unique_ptr<BodyArmour> bodyArmourBase;
	std::unique_ptr<Weapon> pistolBase;
	std::unique_ptr<Weapon> assaultRifleBase;
	std::unique_ptr<Weapon> sniperRifleBase;
	void newCircle();
	int circleNumber;
	void spawnItems();
	void initialiseItems();
	void spawnItemSet(Item& item, std::vector<ItemInstance>& items, int numberToSpawn);
public:
	Game(Level& level);
	void update(float deltaTime);
	bool isPositionInsideNextCircle(glm::vec2 position) const;
	bool isPositionInsideCurrentCircle(glm::vec2 position) const;
	void restart();
	glm::vec2 circleCentre, nextCircleCentre, previousCircleCentre;
	float circleRadius, previousCircleRadius, nextCircleRadius;
	float elapsedShrinkTime;
	Level& levelData;
	float circleDamageTick;
	std::vector<ItemInstance> items;
};