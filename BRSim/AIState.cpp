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