#pragma once

#include <vector>
#include <memory>
#include "glm\gtx\compatibility.hpp"
#include "ItemInstance.h"
#include "Healthpack.h"
#include "Level.h"

const int ITEM_COUNT = 20;

//Responsible for managing the gameplay stuff (tracking circles and other events)
class Game
{
private:
	std::unique_ptr<Healthpack> healthpackBase;
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