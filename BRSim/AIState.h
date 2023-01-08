#pragma once

#include <vector>
#include "Game.h"

class Agent;	//forward declaration

class AIState
{
public:
	virtual void execute(Agent& owner, Game* gameState, float frameTime) = 0;
	void setAgentState(Agent& owner, AIState* newState);
};