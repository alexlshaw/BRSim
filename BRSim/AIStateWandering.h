#pragma once

#include "glm/glm.hpp"
#include "AIState.h"

class AIStateFighting;

class AIStateWandering : public AIState
{
public:
	void execute(Agent& owner, const Game& gameState, float frameTime) override;
};