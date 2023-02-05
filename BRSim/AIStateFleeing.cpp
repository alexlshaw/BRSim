#include "AIStateFighting.h"
#include "AIStateFleeing.h"
#include "AIStateWandering.h"
#include "Agent.h"

void AIStateFleeing::execute(Agent& owner, const Game& gameState, float frameTime)
{
	//Is anyone nearby?
	if (owner.otherVisibleAgents.empty())
	{
		noNearbyEnemies(owner, gameState, frameTime);
	}
	else
	{
		nearbyEnemies(owner, gameState, frameTime);
	}
}

void AIStateFleeing::noNearbyEnemies(Agent& owner, const Game& gameState, float frameTime)
{
	//we're injured, can we see a health pack?
	if (owner.visibleItems.size() > 0)
	{
		owner.setTarget(owner.visibleItems[0].get().position);
	}
	//No enemies, do we have somewhere we want to be?
	if (owner.hasTarget)
	{
		checkAndMoveToTarget(owner, gameState, frameTime);
	}
	//Is there nowhere we want to be (possibly because we just arrived at target in the check/move call above)
	if (!owner.hasTarget)
	{
		setAgentState(owner, new AIStateWandering());
	}
}

void AIStateFleeing::nearbyEnemies(Agent& owner, const Game& gameState, float frameTime)
{
	//Is the circle big enough to run away?
	if (gameState.circleRadius > AGENT_STOP_FLEE_CIRCLE_SIZE)
	{
		//Do we have somewhere to flee to?
		if (owner.hasTarget)
		{
			checkAndMoveToTarget(owner, gameState, frameTime);
		}
		else
		{
			//TODO: Agents probably want to be smarter about where they run to than just picking a random spot
			findTarget(owner, gameState);
		}
	}
	else
	{
		//guess we have to fight
		setAgentState(owner, new AIStateFighting());
	}
	
}