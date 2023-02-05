#pragma once

#include <functional>
#include "glm/glm.hpp"
#include "AIState.h"
#include "AIStateWandering.h"
#include "Entity.h"
#include "Game.h"
#include "Item.h"
#include "Settings.h"

const int AGENT_MAX_HEALTH = 100;
const int AGENT_FLEE_HEALTH_THRESHOLD = 50;	//Agent will try to avoid fights while their health is below this amount
const float AGENT_STOP_FLEE_CIRCLE_SIZE = 360.0f;	//Once the circle gets this small, agents will stop trying to run away from each other and just fight
const float AGENT_MAX_ROTATE_SPEED = 90.0f;
const float AGENT_MAX_SPEED = 20.0f;
const float AGENT_SHOT_COOLDOWN = 1.0f;
const float AGENT_DEFAULT_RANGE = 60.0f;	//default 30

class Agent : public Entity
{
public:
	Agent(glm::vec2 position, float direction, int identity);
	~Agent();
	float look;	//bearing of the agent's look direction (in radians)
	float range;
	float shotCooldownRemainingTime;
	bool firing;	//Whether the agent should spawn a bullet on this/the next frame
	bool alive;
	int currentHealth;
	int id;
	glm::vec2 forward();
	bool hasTarget;
	glm::vec2 targetPosition;	//Location the agent wants to move to unless it is in a fight
	bool rotateTowards(glm::vec2 target, float deltaTime);	//returns true if rotation is enough to face target
	bool moveTowards(glm::vec2 target, float deltaTime);	//returns true if movement is enough to reach target
	void setTarget(glm::vec2 target);
	AIState *currentState;
	void update(float frameTime, const Game& gameState);
	std::vector<std::reference_wrapper<Agent>> otherVisibleAgents;
	std::vector<std::reference_wrapper<const Item>> visibleItems;
	bool activeAndAlive();
};