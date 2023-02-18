#include "AIStateWandering.h"
#include "AIStateFighting.h"
#include "AIStateFleeing.h"
#include "Agent.h"

void AIStateWandering::execute(Agent& owner, const Game& gameState, float frameTime)
{
	//1. Switch to fighting if enemies nearby
	if (!owner.otherVisibleAgents.empty())
	{
		if ((owner.currentHealth > owner.aiWeights.fleeHealthThreshold || gameState.circleRadius <= owner.aiWeights.stopFleeingCircleThreshold) 
			&& owner.currentWeapon.weaponType != WeaponType::none)
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

	//2. Loot if there any interesting items nearby
	if (owner.visibleItems.size() > 0)
	{
		int priorityItemIndex = -1;
		float itemPriority = -1.0f;
		for (unsigned int i = 0; i < owner.visibleItems.size(); i++)
		{
			const ItemInstance& item = owner.visibleItems[i].get();
			float distanceFactor = computeItemDistancePriority(owner, gameState, item);
			float typeFactor = 0.0f;
			switch (item.baseItem.itemType)
			{
			case ItemType::healthpack:
				//the more injured we are, the more of a priority a healthpack is
				typeFactor = computeHealthPriority(owner);
				break;
			case ItemType::bodyArmour:
				//The more armour we're missing, the more we value it.
				typeFactor = computeArmourPriority(owner);
				break;
			default:
				typeFactor = computeWeaponPriority(owner, item);
				break;
			}
			float priority = distanceFactor + typeFactor;
			if (typeFactor > 0.1f && priority > itemPriority)	//Only grab the item if something we care about
			{
				itemPriority = priority;
				priorityItemIndex = i;
			}
		}
		if (priorityItemIndex != -1)
		{
			//we found an item that's worth our time, go for it
			owner.setTarget(owner.visibleItems[priorityItemIndex].get().position, TargetType::item);
		}
	}

	//3. If we haven't started fighting, move to where we want to be
	moveOrFindTarget(owner, gameState, frameTime);
}