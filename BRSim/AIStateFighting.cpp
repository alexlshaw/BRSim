#include "AIStateFighting.h"
#include "AIStateWandering.h"
#include "Agent.h"

void AIStateFighting::execute(Agent& owner, Game* gameState, float frameTime)
{
	//1. Check if we actually have anyone to fight
	if (owner.otherVisibleAgents.empty())
	{
		//switch to fighting state
		setAgentState(owner, new AIStateWandering());
		return;	//Switching state deletes the state object, so we want to wrap up here
	}

	//2. We have 1+ opponents, take a swing at the first one
	Agent& other = owner.otherVisibleAgents[0];
	if (owner.rotateTowards(other.pos, frameTime))
	{
		if (owner.shotCooldownRemainingTime <= 0.0f)
		{
			owner.firing = true;
		}
	}
}