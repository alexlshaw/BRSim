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
	float distanceToNextCircleCentre = glm::length(owner.position - gameState.nextCircleCentre);
	if (distanceToNextCircleCentre > gameState.nextCircleRadius)
	{
		float targetDistance = distanceToNextCircleCentre - (0.95f * gameState.nextCircleRadius);
		owner.setTarget(owner.position + glm::normalize(gameState.nextCircleCentre - owner.position) * targetDistance);
	}
	else
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
		owner.setTarget(randomSpot);
	}
}

void AIState::checkAndMoveToTarget(Agent& owner, const Game& gameState, float frameTime)
{
	if (owner.hasTarget)	//Sanity check, this function shouldn't be called on an agent with no target
	{
		//check if it's still good
		if (!gameState.isPositionInsideNextCircle(owner.targetPosition))
		{
			//if not, cancel it and find a new one
			owner.hasTarget = false;
			findTarget(owner, gameState);
		}
		else
		{
			//Turn and move towards target
			if (owner.rotateTowards(owner.targetPosition, frameTime))
			{
				if (owner.moveTowards(owner.targetPosition, frameTime))
				{
					owner.hasTarget = false;	//if the agent reached its goal, it no longer has a target
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
float AIState::computeItemDistancePriority(const Agent& owner, const ItemInstance& item)
{
	float distance = glm::length(owner.position - item.position);
	return (AGENT_VISIBILITY_RANGE - distance) / AGENT_VISIBILITY_RANGE;
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