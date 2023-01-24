#pragma once

#include <vector>
#include "glm\gtx\compatibility.hpp"
#include "Level.h"

//Responsible for managing the gameplay stuff (tracking circles and other events)

class Game
{
private:
	void newCircle();
public:
	Game(Level& level);
	~Game();
	void update(float deltaTime);
	bool isPositionInsideNextCircle(glm::vec2 position) const;

	glm::vec2 circleCentre, nextCircleCentre, previousCircleCentre;
	float circleRadius, previousCircleRadius, nextCircleRadius;
	float elapsedShrinkTime = 0.0f;
	Level& levelData;
};