#include "Agent.h"

Agent::Agent(glm::vec2 position, float direction)
{
	look = direction;
	pos = position;
	range = AGENT_DEFAULT_RANGE;
	alive = true;
	hasTarget = false;
	targetPosition = position;
}

Agent::~Agent() {}

glm::vec2 Agent::forward()
{
	//this should be normalized by how it is computed
	return glm::vec2(glm::cos(look), glm::sin(look));
}

bool Agent::rotateTowards(glm::vec2 targetLocation, float deltaTime)
{
	glm::vec2 target = glm::normalize(targetLocation - pos);	//the direction vector the agent now wants
	glm::vec2 fwd = forward();
	float d = glm::dot(target, fwd);
	if (d == 0.0f || glm::length(target - fwd) < 0.01f)	//if we are facing/very close to facing, the target, just stop there
	{
		return true;
	}
	float angleBetween = glm::acos(d);
	//check which way we actually have to rotate
	float side = (fwd.x * target.y) - (fwd.y * target.x);
	//rotate to face target (if possible)
	float maxRotation = glm::radians(AGENT_MAX_ROTATE_SPEED) * deltaTime;
	//printf("Fwd: %.2f, %.2f || tgt: %.2f, %.2f || Angle: %.2f\n", fwd.x, fwd.y, target.x, target.y, glm::degrees(angleBetween));
	if (angleBetween > maxRotation)
	{
		look += maxRotation * (side < 0.0f ? -1.0f : 1.0f); //if forward vector is left of target vector -> rotate clockwise
		return false;
	}
	else
	{
		//we're close enough to directly face the target location
		look += angleBetween * (side < 0.0f ? -1.0f : 1.0f); //if forward vector is left of target vector -> rotate clockwise
		return true;
	}
	
}

bool Agent::moveTowards(glm::vec2 targetLocation, float deltaTime)
{
	//simple case: we assume that if the agent is moving towards a point, it is ~facing that point
	float len = glm::length(targetLocation - pos);
	float maxDistance = deltaTime * AGENT_MAX_SPEED;
	if (len > maxDistance)
	{
		pos = pos + forward() * maxDistance;
		return false;
	}
	else
	{
		pos = targetLocation;
		return true;
	}
}

void Agent::setTarget(glm::vec2 target)
{
	targetPosition = target;
	hasTarget = true;
}