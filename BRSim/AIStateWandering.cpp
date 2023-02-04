#include "AIStateWandering.h"
#include "AIStateFighting.h"
#include "AIStateFleeing.h"
#include "Agent.h"

void AIStateWandering::execute(Agent& owner, const Game& gameState, float frameTime)
{
	//1. Switch to fighting if enemies nearby
	if (!owner.otherVisibleAgents.empty())
	{
		if (owner.currentHealth > AGENT_FLEE_HEALTH_THRESHOLD || gameState.circleRadius <= AGENT_STOP_FLEE_CIRCLE_SIZE)
		{
			//switch to fighting state
			setAgentState(owner, new AIStateFighting());
		}
		else
		{
			//run away
			setAgentState(owner, new AIStateFleeing());
		}
		return;	//Switching state deletes the state object, so we want to wrap up here
	}
	//2. Are we injured and able to see a health pack?
	if (owner.currentHealth < AGENT_MAX_HEALTH)
	{
		//can we see a health pack?
		if (owner.visibleItems.size() > 0)
		{
			owner.setTarget(owner.visibleItems[0].get().location);
		}
	}

	//3. If not, continue moving until we reach target, then pick a new target
	if (owner.hasTarget)
	{
		checkAndMoveToTarget(owner, gameState, frameTime);
	}
	else
	{
		findTarget(owner, gameState);
	}
}