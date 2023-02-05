#pragma once

#include <vector>
#include "Game.h"
#include "ItemInstance.h"

class Agent;	//forward declaration

class AIState
{
protected:
	//Priority computations are all virtual because it is very plausible that the current state influences how we calculate priorities
	virtual float computeItemDistancePriority(const Agent& owner, const ItemInstance& item);
	virtual float computeHealthPriority(const Agent& owner);
	virtual float computeArmourPriority(const Agent& owner);

	void setAgentState(Agent& owner, AIState* newState);
	void findTarget(Agent& owner, const Game& gameState);
	void checkAndMoveToTarget(Agent& owner, const Game& gameState, float frameTime);
public:
	virtual void execute(Agent& owner, const Game& gameState, float frameTime) = 0;
};