#include "AIStateFighting.h"
#include "AIStateFleeing.h"
#include "AIStateWandering.h"
#include "Agent.h"

void AIStateFighting::execute(Agent& owner, const Game& gameState, float frameTime)
{
	//1. Check if we actually have anyone to fight
	if (owner.otherVisibleAgents.empty())
	{
		//switch to wandering state
		owner.firing = false;
		setAgentState(owner, new AIStateWandering());
		return;	//Switching state deletes the state object, so we want to wrap up here
	}
	
	//2. We have 1+ opponents, do we want this fight?
	if (owner.currentHealth > owner.aiWeights.fleeHealthThreshold || gameState.circleRadius <= owner.aiWeights.stopFleeingCircleThreshold)
	{
		Agent& other = determinePreferredTarget(owner);
		glm::vec2 shootAt = owner.computeTargetInterceptionPoint(other.position, other.currentVelocity());
		if (owner.rotateTowards(shootAt, frameTime))
		{
			if (glm::length(owner.position - other.position) < owner.currentWeapon.range)
			{
				if (owner.shotCooldownRemainingTime <= 0.0f)
				{
					owner.firing = true;
				}
			}
			else
			{
				owner.moveTowards(other.position, frameTime);
			}
		}
	}
	else
	{
		//we do not want this fight
		owner.firing = false;
		setAgentState(owner, new AIStateFleeing());
		return;	//Switching state deletes the state object, so we want to wrap up here
	}
	
}

//Of the agents we can see, which one do we want to shoot most? For now, just the one that requires the least rotation to face
Agent& AIStateFighting::determinePreferredTarget(const Agent& owner)
{
	float closestIndex = -1;
	float closestAngle = glm::pi<float>();

	for (int i = 0; i < owner.otherVisibleAgents.size(); i++) {
		glm::vec2 toHostile = owner.otherVisibleAgents[i].get().position - owner.position;
		float distance = glm::length(toHostile);

		if (distance < 0.001f) {
			continue; // ignore entities at the same position as the entity
		}

		toHostile /= distance;

		float angle = glm::angle(owner.forward(), toHostile);

		if (angle < closestAngle) {
			closestAngle = angle;
			closestIndex = i;
		}
	}
	return owner.otherVisibleAgents[closestIndex];
}