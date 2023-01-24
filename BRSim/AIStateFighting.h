#pragma once

#include "AIState.h"

class AIStateFighting : public AIState
{
public:
	void execute(Agent& owner, const Game& gameState, float frameTime) override;
};