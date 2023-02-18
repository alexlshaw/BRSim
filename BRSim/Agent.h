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
private:
	float speedLastFrame;	//How fast the agent travelled in the previous frame in units/s
public:
	Agent(glm::vec2 position, float direction, int identity);
	~Agent();
	glm::vec2 forward() const;
	glm::vec2 currentVelocity() const;
	bool rotateTowards(glm::vec2 target, float deltaTime);	//returns true if rotation is enough to face target
	bool moveTowards(glm::vec2 target, float deltaTime);	//returns true if movement is enough to reach target
	void setTarget(glm::vec2 target, TargetType targetType);
	void update(const float frameTime, const Game& gameState);
	bool activeAndAlive() const;
	void reset();
	glm::vec2 computeTargetInterceptionPoint(glm::vec2 targetCurrentLocation, glm::vec2 targetVelocity) const;
	float look;	//bearing of the agent's look direction (in radians)
	float shotCooldownRemainingTime;
	bool firing;	//Whether the agent should spawn a bullet on the next update cycle
	bool alive;
	float currentHealth;
	float currentArmour;
	int id;
	float survivalTime;
	int killCount;
	TargetType currentTargetType;
	glm::vec2 targetPosition;	//Location the agent wants to move to unless it is in a fight
	AIState *currentState;
	std::vector<std::reference_wrapper<Agent>> otherVisibleAgents;
	std::vector<std::reference_wrapper<const ItemInstance>> visibleItems;
	EquippedWeapon currentWeapon;
	AIWeights aiWeights;
};