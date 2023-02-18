#pragma once

#include "AIState.h"
#include "glm\gtx\vector_angle.hpp"

class AIStateFighting : public AIState
{
protected:
	virtual Agent& determinePreferredTarget(const Agent& owner);
public:
	void execute(Agent& owner, const Game& gameState, float frameTime) override;
};