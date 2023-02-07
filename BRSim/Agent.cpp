#include "Agent.h"

Agent::Agent(glm::vec2 position, float direction, int identity)
	:Entity(position), 
	look(direction),
	firing(false), 
	alive(true), 
	currentTargetType(TargetType::none),
	targetPosition(position),
	id(identity),
	shotCooldownRemainingTime(0.0f),
	currentHealth(AGENT_MAX_HEALTH),
	currentArmour(0),
	currentState(static_cast<AIState*>(new AIStateWandering())),
	aiWeights(AIWeights())
{
	//TODO: This currently leaks memory -> AI state on game end not deleted. Currently a non-issue in that game end also means process end, but should fix
}

Agent::~Agent() {}

glm::vec2 Agent::forward()
{
	//this should be normalized by how it is computed
	return glm::vec2(glm::cos(look), glm::sin(look));
}

bool Agent::rotateTowards(glm::vec2 targetLocation, float deltaTime)
{
	glm::vec2 target = glm::normalize(targetLocation - position);	//the direction vector the agent now wants
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
	float len = glm::length(targetLocation - position);
	float maxDistance = deltaTime * AGENT_MAX_SPEED;
	if (len > maxDistance)
	{
		position = position + forward() * maxDistance;
		return false;
	}
	else
	{
		position = targetLocation;
		return true;
	}
}

void Agent::setTarget(glm::vec2 target, TargetType targetType)
{
	targetPosition = target;
	currentTargetType = targetType;
}

void Agent::update(float frameTime, const Game& gameState)
{
	//update the agent's internal state, then figure out what it wants to do next
	shotCooldownRemainingTime = glm::max<float>(0.0f, shotCooldownRemainingTime - frameTime);
	currentState->execute(*this, gameState, frameTime);
}

bool Agent::activeAndAlive()
{
	return enabled && alive;
}