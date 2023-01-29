#include "AIStateFighting.h"
#include "AIStateFleeing.h"
#include "AIStateWandering.h"
#include "Agent.h"

void AIStateFighting::execute(Agent& owner, const Game& gameState, float frameTime)
{
	//1. Check if we actually have anyone to fight
	if (owner.otherVisibleAgents.empty())
	{
		//switch to wandering state
		owner.firing = false;
		setAgentState(owner, new AIStateWandering());
		return;	//Switching state deletes the state object, so we want to wrap up here
	}

	//2. We have 1+ opponents, do we want this fight?
	if (owner.currentHealth > AGENT_FLEE_HEALTH_THRESHOLD || gameState.circleRadius <= AGENT_STOP_FLEE_CIRCLE_SIZE)
	{
		Agent& other = owner.otherVisibleAgents[0];
		if (owner.rotateTowards(other.pos, frameTime))
		{
			if (owner.shotCooldownRemainingTime <= 0.0f)
			{
				owner.firing = true;
			}
		}
	}
	else
	{
		//we do not want this fight
		owner.firing = false;
		setAgentState(owner, new AIStateFleeing());
		return;	//Switching state deletes the state object, so we want to wrap up here
	}
	
}