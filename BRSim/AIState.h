#pragma once

#include <vector>
#include "Game.h"
#include "ItemInstance.h"
#include "EquippedWeapon.h"

class Agent;	//forward declaration

class AIState
{
protected:
	//Priority computations are all virtual because it is very plausible that the current state influences how we calculate priorities
	virtual float computeItemDistancePriority(const Agent& owner, const Game& gameState, const ItemInstance& item);
	virtual float computeHealthPriority(const Agent& owner);
	virtual float computeArmourPriority(const Agent& owner);
	virtual float computeWeaponPriority(const Agent& owner, const ItemInstance& item);

	void setAgentState(Agent& owner, AIState* newState);
	void findTarget(Agent& owner, const Game& gameState);
	void moveOrFindTarget(Agent& owner, const Game& gameState, float frameTime);
	void checkAndMoveToTarget(Agent& owner, const Game& gameState, float frameTime);
public:
	virtual void execute(Agent& owner, const Game& gameState, float frameTime) = 0;
};