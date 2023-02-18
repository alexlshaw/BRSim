#include "AIState.h"
#include "Agent.h"	//Agent needs to be included here as using forward declaration

void AIState::setAgentState(Agent& owner, AIState* newState)
{
	AIState* oldState = owner.currentState;
	owner.currentState = newState;
	delete oldState;
}

void AIState::findTarget(Agent& owner, const Game& gameState)
{
	if (gameState.isPositionInsideNextCircle(owner.position))
	{
		//agent is in the next circle, just pick a random spot (within the next circle)
		float rTheta = glm::radians((float)(rand() % 360));
		float rRad = (float)(rand() % (int)(gameState.nextCircleRadius));
		float rx = glm::cos(rTheta) * rRad;
		float ry = glm::sin(rTheta) * rRad;
		glm::vec2 randomSpot = gameState.nextCircleCentre + glm::vec2(rx, ry);
		//make sure the spot in question is actually within the level
		randomSpot.x = glm::clamp(randomSpot.x, 0.0f, (float)gameState.levelData.width);
		randomSpot.y = glm::clamp(randomSpot.y, 0.0f, (float)gameState.levelData.height);
		owner.setTarget(randomSpot, TargetType::position);
	}
	else
	{
		//we're outside of the next circle, our target is just to get inside the closest edge
		float distanceToNextCircleCentre = glm::length(owner.position - gameState.nextCircleCentre);
		float targetDistance = distanceToNextCircleCentre - (0.95f * gameState.nextCircleRadius);
		owner.setTarget(owner.position + glm::normalize(gameState.nextCircleCentre - owner.position) * targetDistance, TargetType::position);
	}
}

void AIState::moveOrFindTarget(Agent& owner, const Game& gameState, float frameTime)
{
	//Move to the target if we have one, otherwise find one
	if (owner.currentTargetType != TargetType::none)
	{
		checkAndMoveToTarget(owner, gameState, frameTime);
	}
	//Is there nowhere we want to be (possibly because we just arrived at target in the check/move call above)
	if (owner.currentTargetType == TargetType::none)
	{
		findTarget(owner, gameState);
	}
}

void AIState::checkAndMoveToTarget(Agent& owner, const Game& gameState, float frameTime)
{
	if (owner.currentTargetType != TargetType::none)	//Sanity check, this function shouldn't be called on an agent with no target
	{
		//If it's a position, make sure it's inside the circle
		if (owner.currentTargetType == TargetType::position && !gameState.isPositionInsideNextCircle(owner.targetPosition))
		{
			//if not, cancel it and find a new one
			owner.currentTargetType = TargetType::none;
			findTarget(owner, gameState);
		}
		else
		{
			//if it's an item or in a safe location, go for it
			if (owner.rotateTowards(owner.targetPosition, frameTime))
			{
				if (owner.moveTowards(owner.targetPosition, frameTime))
				{
					owner.currentTargetType = TargetType::none;	//if the agent reached its goal, it no longer has a target
				}
			}
		}
	}
	else
	{
		printf("Agent %i trying to move to non-existent target\n", owner.id);
	}
}

//How importance is picking up an item based on its current distance? (Range 0...1)
//This should range from 1.0 if the item is co-located with the agent, to 0.0 if the item is at the agent's maximum view range
float AIState::computeItemDistancePriority(const Agent& owner, const Game& gameState, const ItemInstance& item)
{
	float distance = glm::length(owner.position - item.position);
	float priority = ((AGENT_VISIBILITY_RANGE - distance) / AGENT_VISIBILITY_RANGE) * owner.aiWeights.itemDistancePriority;
	if (!gameState.isPositionInsideNextCircle(item.position))
	{
		priority *= owner.aiWeights.itemNotInNextCirclePriorityModifier;
		if (!gameState.isPositionInsideCurrentCircle(item.position))
		{
			priority *= owner.aiWeights.itemNotInCurrentCirclePriorityModifier;
		}
	}
	return priority;
}

//how important are health pickups for us? (Range 0...1)
float AIState::computeHealthPriority(const Agent& owner)
{
	//how much would a healthpack give the agent right now?
	float possibleHeal = glm::min<float>(AGENT_MAX_HEALTH - owner.currentHealth, HEALTHPACK_HEAL_AMOUNT);
	return (possibleHeal / AGENT_MAX_HEALTH) * owner.aiWeights.healthItemPriority;
}

//how important are armour pickups for us? (Range 0...0.5)
//Armour priority caps at 0.5f because it only protects with 50% efficiency
//So the equivalent value of health will be valued twice as much
float AIState::computeArmourPriority(const Agent& owner)
{
	//how much would a body armour give right now?
	float possibleArmour = glm::min<float>(AGENT_MAX_ARMOUR - owner.currentArmour, BODYARMOUR_ARMOUR_AMOUNT);
	return (possibleArmour / AGENT_MAX_ARMOUR * 0.5f) * owner.aiWeights.armourItemPriority;
}

//weapon priority depends on the relative quality of the weapon compared to the weapon we currently have
//Range(-10...1)
float AIState::computeWeaponPriority(const Agent& owner, const ItemInstance& item)
{
	WeaponType weaponType = dynamic_cast<Weapon&>(item.baseItem).awardedWeapon.weaponType;
	return owner.aiWeights.swapWeaponPriorities[(int)owner.currentWeapon.weaponType][(int)weaponType];
}