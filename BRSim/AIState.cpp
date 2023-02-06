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
	float priority = (AGENT_VISIBILITY_RANGE - distance) / AGENT_VISIBILITY_RANGE;
	if (!gameState.isPositionInsideNextCircle(item.position))
	{
		priority *= 0.5f;
		if (!gameState.isPositionInsideNextCircle(item.position))
		{
			priority *= 0.5f;
		}
	}
	return priority;
}

//how important are health pickups for us? (Range 0...1)
float AIState::computeHealthPriority(const Agent& owner)
{
	//how much would a healthpack give the agent right now?
	float possibleHeal = glm::min<float>(AGENT_MAX_HEALTH - owner.currentHealth, HEALTHPACK_HEAL_AMOUNT);
	return possibleHeal / AGENT_MAX_HEALTH;
}

//how important are armour pickups for us? (Range 0...0.5)
//Armour priority caps at 0.5f because it only protects with 50% efficiency
//So the equivalent value of health will be valued twice as much
float AIState::computeArmourPriority(const Agent& owner)
{
	//how much would a body armour give right now?
	float possibleArmour = glm::min<float>(AGENT_MAX_ARMOUR - owner.currentArmour, BODYARMOUR_ARMOUR_AMOUNT);
	return possibleArmour / AGENT_MAX_ARMOUR * 0.5f;
}

//weapon priority depends on the relative quality of the weapon compared to the weapon we currently have
//Range(-10...1)
float AIState::computeWeaponPriority(const Agent& owner, const ItemInstance& item)
{
	WeaponType weaponType = dynamic_cast<Weapon&>(item.baseItem).awardedWeapon.weaponType;
	//simple increasing scale based on the type of the weapon
	float typeScale = 0.0f;
	switch (weaponType)
	{
	case WeaponType::pistol:
		typeScale = 0.8f;
		break;
	case WeaponType::assaultRifle:
		typeScale = 0.9f;
		break;
	case WeaponType::sniperRifle:
		typeScale = 1.0f;
		break;
	}

	float priority = 0.0f;
	//if we already have this gun, we don't care about it
	if (weaponType == owner.currentWeapon.weaponType)
	{
		priority = 0.0f;
	}
	else
	{
		//figure out the weapon relative power
		switch (owner.currentWeapon.weaponType)
		{
		case WeaponType::none:
			//if we're unarmed, boy howdy do we want a gun. We still scale based on type in case we can see several
			priority = typeScale;
			break;
		case WeaponType::pistol:
			//Other gun is something better than a pistol, we want to get it but not as much as we would if we were unarmed
			priority = 0.75f * typeScale;
			break;
		case WeaponType::assaultRifle:
			//Don't swap if it's a pistol. If it's a sniper, we want to swap but it's not too high a priority
			priority = weaponType == WeaponType::pistol ? -10.0f : 0.5f * typeScale;
			break;
		case WeaponType::sniperRifle:
			//we have the best gun (later we'll probably modify this based on progress through the game). Don't swap.
			priority = -10.0f;
			break;
		}
	}
	return priority;
}