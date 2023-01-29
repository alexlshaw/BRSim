#pragma once

#include "AIState.h"

class AIStateFleeing : public AIState
{
	void nearbyEnemies(Agent& owner, const Game& gameState, float frameTime);	//Actions to execute if enemies are nearby
	void noNearbyEnemies(Agent& owner, const Game& gameState, float frameTime); //Actions to execute if no enemies are nearby
public:
	void execute(Agent& owner, const Game& gameState, float frameTime) override;
};