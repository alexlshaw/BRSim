#pragma once

#include <iostream>
#include "glm/glm.hpp"
#include "AIState.h"

class AIStateFighting;

class AIStateWandering : public AIState
{
private:
	void findTarget(Agent& owner, Game* gameState);
public:
	void execute(Agent& owner, Game* gameState, float frameTime) override;
};