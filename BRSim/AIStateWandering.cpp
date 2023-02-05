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
	//2. Are there any interesting items nearby?
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
				//the more injured we are, the more of a priority a healthpack is
				typeFactor = computeHealthPriority(owner);
				break;
			case BODYARMOUR:
				//The more armour we're missing, the more we value it.
				typeFactor = computeArmourPriority(owner);
				break;
			default:
				//TODO: gun priority decreases the more we value health or armour, and changes based on how well equipped we already are
				typeFactor = 0.2f;
				break;
			}
			float priority = distanceFactor + typeFactor;
			if (typeFactor > 0.1f && priority > itemPriority)	//Only grab the item if something we care about
			{
				itemPriority = priority;
				priorityItemIndex = i;
			}
		}
		if (priorityItemIndex != -1.0f)
		{
			//we found an item that's worth our time, go for it
			owner.setTarget(owner.visibleItems[priorityItemIndex].get().position);
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