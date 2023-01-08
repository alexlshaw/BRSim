#include "AIStateWandering.h"
#include "AIStateFighting.h"
#include "Agent.h"

void AIStateWandering::execute(Agent& owner, Game* gameState, float frameTime)
{
	//1. Switch to fighting if enemies nearby
	if (!owner.otherVisibleAgents.empty())
	{
		//switch to fighting state
		setAgentState(owner, new AIStateFighting());
		return;	//Switching state deletes the state object, so we want to wrap up here
	}

	//2. If not, continue moving until we reach target, then pick a new target
	if (owner.hasTarget)
	{
		//check if it's still good
		if (!gameState->isPositionInsideNextCircle(owner.targetPosition))
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
		findTarget(owner, gameState);
	}
}

void AIStateWandering::findTarget(Agent& owner, Game* gameState)
{
	float distanceToNextCircleCentre = glm::length(owner.pos - gameState->nextCircleCentre);
	if (distanceToNextCircleCentre > gameState->nextCircleRadius)
	{
		float targetDistance = distanceToNextCircleCentre - (0.95f * gameState->nextCircleRadius);
		owner.setTarget(owner.pos + glm::normalize(gameState->nextCircleCentre - owner.pos) * targetDistance);
	}
	else
	{
		//agent is in the next circle, just pick a random spot (within the next circle)
		float rTheta = glm::radians((float)(rand() % 360));
		float rRad = (float)(rand() % (int)(gameState->nextCircleRadius));
		float rx = glm::cos(rTheta) * rRad;
		float ry = glm::sin(rTheta) * rRad;
		glm::vec2 randomSpot = gameState->nextCircleCentre + glm::vec2(rx, ry);
		//make sure the spot in question is actually within the level
		randomSpot.x = glm::clamp(randomSpot.x, 0.0f, (float)gameState->levelData.width);
		randomSpot.y = glm::clamp(randomSpot.y, 0.0f, (float)gameState->levelData.height);
		owner.setTarget(randomSpot);
	}
}