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
	//we're injured, are there items available?
	if (owner.visibleItems.size() > 0)
	{
		int priorityItemIndex = -1;
		float itemPriority = -1.0f;
		for (unsigned int i = 0; i < owner.visibleItems.size(); i++)
		{
			const ItemInstance& item = owner.visibleItems[i].get();
			float distanceFactor = computeItemDistancePriority(owner, item);
			float typeFactor = 0.0f;
			switch (item.baseItem.itemType)
			{
			case HEALTHPACK:
				//the more injured we are, the more of a priority a healthpack is. Range 0...1
				typeFactor = computeHealthPriority(owner);
				break;
			case BODYARMOUR:
				//The more armour we're missing, the more we value it. But only at half the relative value of health. Range 0...0.5
				typeFactor = computeArmourPriority(owner);
				break;
			default:
				//any guns are low priority while we're fleeing
				typeFactor = 0.1f;
				break;
			}
			float priority = distanceFactor + typeFactor;
			if (priority > itemPriority)
			{
				itemPriority = priority;
				priorityItemIndex = i;
			}
		}
		owner.setTarget(owner.visibleItems[priorityItemIndex].get().position);
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