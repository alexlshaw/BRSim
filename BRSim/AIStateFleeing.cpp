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
			float distanceFactor = computeItemDistancePriority(owner, gameState, item);
			float typeFactor = 0.0f;
			switch (item.baseItem.itemType)
			{
			case ItemType::healthpack:
				//the more injured we are, the more of a priority a healthpack is. Range 0...1
				typeFactor = computeHealthPriority(owner);
				break;
			case ItemType::bodyArmour:
				//The more armour we're missing, the more we value it. But only at half the relative value of health. Range 0...0.5
				typeFactor = computeArmourPriority(owner);
				break;
			default:
				//any guns are low priority while we're fleeing
				typeFactor = computeWeaponPriority(owner, item) * owner.aiWeights.weaponPickupWhileFleeingPriority;
				break;
			}
			float priority = distanceFactor + typeFactor;
			//if this item is better than others we've looked at (and also worth our time)
			if (priority > itemPriority && priority > owner.aiWeights.ignoreItemThreshold)
			{
				itemPriority = priority;
				priorityItemIndex = i;
			}
		}
		if (priorityItemIndex != -1)
		{
			owner.setTarget(owner.visibleItems[priorityItemIndex].get().position, TargetType::item);
		}
	}
	//No enemies or items, do we have somewhere we want to be?
	if (owner.currentTargetType != TargetType::none)
	{
		checkAndMoveToTarget(owner, gameState, frameTime);
	}
	else
	{
		//we've reached wherever we wanted to flee to, resume wandering
		setAgentState(owner, new AIStateWandering());
	}
}

void AIStateFleeing::nearbyEnemies(Agent& owner, const Game& gameState, float frameTime)
{
	//is the circle too small to run (and we are able to fight)?
	if (gameState.circleRadius <= owner.aiWeights.stopFleeingCircleThreshold && owner.currentWeapon.weaponType != WeaponType::none)
	{
		//guess we'd better fight
		setAgentState(owner, new AIStateFighting());
	}
	else
	{
		//keep running
		moveOrFindTarget(owner, gameState, frameTime);
	}
}