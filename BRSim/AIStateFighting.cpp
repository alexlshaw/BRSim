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
	if (owner.currentHealth > owner.aiWeights.fleeHealthThreshold || gameState.circleRadius <= owner.aiWeights.stopFleeingCircleThreshold)
	{
		Agent& other = owner.otherVisibleAgents[0];
		if (owner.rotateTowards(other.position, frameTime))
		{
			if (glm::length(owner.position - other.position) < owner.currentWeapon.range)
			{
				if (owner.shotCooldownRemainingTime <= 0.0f)
				{
					owner.firing = true;
				}
			}
			else
			{
				owner.moveTowards(other.position, frameTime);
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