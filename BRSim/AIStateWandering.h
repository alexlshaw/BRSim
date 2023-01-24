#pragma once

#include <iostream>
#include "glm/glm.hpp"
#include "AIState.h"

class AIStateFighting;

class AIStateWandering : public AIState
{
private:
	void findTarget(Agent& owner, const Game& gameState);
public:
	void execute(Agent& owner, const Game& gameState, float frameTime) override;
};