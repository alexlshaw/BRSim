#pragma once

#include <functional>
#include "glm/glm.hpp"
#include "AIState.h"
#include "AIStateWandering.h"
#include "AIWeights.h"
#include "Entity.h"
#include "EquippedWeapon.h"
#include "Game.h"
#include "ItemInstance.h"

const float AGENT_MAX_HEALTH = 100.0f;
const float AGENT_MAX_ARMOUR = 100.0f;
const float AGENT_MAX_ROTATE_SPEED = 90.0f;
const float AGENT_MAX_SPEED = 20.0f;
const float AGENT_VISIBILITY_RANGE = 100.0f;

enum class TargetType
{
	none,
	position,
	item
};

class Agent : public Entity
{
public:
	Agent(glm::vec2 position, float direction, int identity);
	~Agent();
	float look;	//bearing of the agent's look direction (in radians)
	float shotCooldownRemainingTime;
	bool firing;	//Whether the agent should spawn a bullet on the next update cycle
	bool alive;
	float currentHealth;
	float currentArmour;
	int id;
	glm::vec2 forward();
	TargetType currentTargetType;
	glm::vec2 targetPosition;	//Location the agent wants to move to unless it is in a fight
	bool rotateTowards(glm::vec2 target, float deltaTime);	//returns true if rotation is enough to face target
	bool moveTowards(glm::vec2 target, float deltaTime);	//returns true if movement is enough to reach target
	void setTarget(glm::vec2 target, TargetType targetType);
	AIState *currentState;
	void update(float frameTime, const Game& gameState);
	std::vector<std::reference_wrapper<Agent>> otherVisibleAgents;
	std::vector<std::reference_wrapper<const ItemInstance>> visibleItems;
	EquippedWeapon currentWeapon;
	bool activeAndAlive() const;
	void reset();
	AIWeights aiWeights;
};