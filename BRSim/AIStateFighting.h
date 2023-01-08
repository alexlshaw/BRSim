#pragma once

#include <iostream>
#include "AIState.h"

class AIStateFighting : public AIState
{
public:
	void execute(Agent& owner, Game* gameState, float frameTime) override;
};