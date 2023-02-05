#pragma once

#include <vector>
#include <memory>
#include "glm\gtx\compatibility.hpp"
#include "ItemInstance.h"
#include "Healthpack.h"
#include "BodyArmour.h"
#include "Level.h"

const int HEALTHPACK_COUNT = 20;
const int ARMOUR_COUNT = 15;
const float CIRCLE_SHRINK_TIME = 20.0f;		//default 20

//Responsible for managing the gameplay stuff (tracking circles and other events)
class Game
{
private:
	std::unique_ptr<Healthpack> healthpackBase;
	std::unique_ptr<BodyArmour> bodyArmourBase;
	void newCircle();
	int circleNumber;
	void spawnItems();
public:
	Game(Level& level);
	~Game();
	void update(float deltaTime);
	bool isPositionInsideNextCircle(glm::vec2 position) const;
	glm::vec2 circleCentre, nextCircleCentre, previousCircleCentre;
	float circleRadius, previousCircleRadius, nextCircleRadius;
	float elapsedShrinkTime = 0.0f;
	Level& levelData;
	float circleDamageTick;
	std::vector<ItemInstance> items;
};