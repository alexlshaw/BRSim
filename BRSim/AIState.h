#pragma once

#include <vector>
#include "Game.h"

class Agent;	//forward declaration

class AIState
{
public:
	virtual void execute(Agent& owner, const Game& gameState, float frameTime) = 0;
	void setAgentState(Agent& owner, AIState* newState);
	void findTarget(Agent& owner, const Game& gameState);
	void checkAndMoveToTarget(Agent& owner, const Game& gameState, float frameTime);
};