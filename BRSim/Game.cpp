#include "Game.h"

Game::Game(Level& level)
	: levelData(level)
{
	spawnItems();
	circleNumber = 1;
	circleDamageTick = 1.0f;
	circleCentre = glm::vec2(level.width / 2.0f, level.height / 2.0f);
	circleRadius = (float)glm::max<int>(level.width, level.height) * 1.5f;
	previousCircleRadius = circleRadius;
	previousCircleCentre = circleCentre;
	newCircle();
}

Game::~Game() {}

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
	nextCircleRadius = circleRadius / 2.0f;
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

void Game::spawnItems()
{
	items.reserve(ITEM_COUNT);
	for (int i = 0; i < ITEM_COUNT; i++)
	{
		bool valid = false;
		glm::vec2 pos;
		while (!valid)
		{
			float x = (float)(rand() % (int)levelData.width);
			float y = (float)(rand() % (int)levelData.height);
			pos = glm::vec2(x, y);
			valid = levelData.getLevelInfo(x, y).walkable;
		}
		items.push_back(Item(pos));
	}
}