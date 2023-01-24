#pragma once

#include <vector>
#include "glm/glm.hpp"
#include "glm\gtx\transform.hpp"
#include "glm\gtx\compatibility.hpp"
#include "Mesh.h"
#include "Level.h"
#include "Settings.h"
#include "Shader.h"
#include "Vertex.h"

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