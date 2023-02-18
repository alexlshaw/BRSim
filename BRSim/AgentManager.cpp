#include "AgentManager.h"

AgentManager::AgentManager(int agentCount, Level& level)
	: levelData(level), agentsAlive(agentCount), elapsedDamageTickTime(0.0f) {}

AgentManager::~AgentManager() {}

void AgentManager::updateAgents(float frameTime, Game& gameState)
{
	elapsedDamageTickTime += frameTime;
	if (elapsedDamageTickTime > 1.0f)
	{
		elapsedDamageTickTime -= 1.0f;
		hurtAgentsOutsideCircle(gameState);
	}
	for (auto& agent : agents)
	{
		if (agent.activeAndAlive())
		{
			updateAgentSightOfOtherAgents(agent);
			updateAgentSightOfItems(agent, gameState);
			checkPickups(agent, gameState);
			agent.update(frameTime, gameState);
			checkAndResolveAgentCollisions(agent);
			//Check agent-agent collisions
			if (agent.firing)
			{
				fireBullet(agent);
			}
		}
	}
}

void AgentManager::fireBullet(Agent& shooter)
{
	Bullet shot(shooter.position + AGENT_COLLISION_RADIUS * shooter.forward(),
		shooter.forward(),
		shooter.currentWeapon.range,
		shooter.currentWeapon.bulletDamage,
		shooter.currentWeapon.bulletSpeed,
		&shooter);
	bullets.push_back(shot);
	shooter.shotCooldownRemainingTime = shooter.currentWeapon.timeBetweenShots;
	shooter.firing = false;
}

void AgentManager::updateBullets(float frameTime)
{
	for (auto& bullet : bullets)
	{
		float minHitT = 99999.0f;
		int hitAgentIdx = -1;
		for (unsigned int j = 0; j < agents.size(); j++)
		{
			if (agents[j].activeAndAlive())
			{
				float t = checkCollision(bullet, agents[j], frameTime);
				if (t != 0.0f && t < minHitT)
				{
					minHitT = t;
					hitAgentIdx = j;
				}
			}
		}
		//If we've hit something, process the hit
		if (hitAgentIdx != -1)
		{
			hitAgentWithBullet(agents[hitAgentIdx], bullet);
		}
		//update position and other bullet internals
		bullet.update(frameTime);
	}
	auto it = std::remove_if(bullets.begin(), bullets.end(), [](Bullet& x) { return !x.enabled; });
	bullets.erase(it, bullets.end());
}

float AgentManager::checkCollision(Bullet& bullet, Agent& agent, float frameTime)
{
	//we want to compute the line of the bullet's travel this frame, and determine whether this line will pass through something
	glm::vec2 endPos = bullet.position + bullet.direction * frameTime * bullet.bulletSpeed;
	glm::vec2 trajectory = endPos - bullet.position;
	glm::vec2 AC = agent.position - bullet.position;
	//project AC onto trajectory
	float pt = glm::dot(trajectory, AC) / glm::length(trajectory);
	//make sure the target is actually along the section of the trajectory line that the bullet will travel in this frame
	if (pt < 0.0f || pt > glm::length(trajectory))
	{
		return false;
	}
	glm::vec2 closestPoint = bullet.position + pt * glm::normalize(trajectory);
	if (glm::length(agent.position - closestPoint) < AGENT_COLLISION_RADIUS)
	{
		if (bullet.shooter->id == agent.id)
		{
			printf("Stop hitting yourself\n");
		}
		//it's a hit
		return pt;
	}
	return 0.0f;
}

void AgentManager::checkPickups(Agent& agent, Game& gameState)
{
	for (auto& item : gameState.items)
	{
		if (item.enabled && glm::length(agent.position - item.position) < ITEM_COLLISION_RADIUS + AGENT_COLLISION_RADIUS)
		{
			item.onPickup(agent);
		}
	}
}

void AgentManager::checkAndResolveAgentCollisions(Agent& agent)
{
	//Agent-agent collisions
	for (auto& otherAgent : agents)
	{
		if (otherAgent.id != agent.id && otherAgent.activeAndAlive())
		{
			glm::vec2 displacement = otherAgent.position - agent.position;
			float displacementLength = glm::length(displacement);
			//handle the case where the agents are not just overlapping, but co-located
			if (displacementLength == 0.0f)
			{
				displacement = glm::vec2(0.0f, 1.0f);
			}
			//check if they should be pushed away from each other
			if (displacementLength < 2.0f * AGENT_COLLISION_RADIUS)
			{
				//they're too close
				glm::vec2 midPoint = agent.position + (0.5f * displacement);
				glm::vec2 push = glm::normalize(displacement) * AGENT_COLLISION_RADIUS;
				//Move this agent away from the midpoint, in the direction of the vector from the midpoint to its current position
				agent.position = midPoint - push;
				//Move the other agent away from the midpoint, in the direction of the vector from the midpoint to its current position
				otherAgent.position = midPoint + push;
			}
		}
	}
}

void AgentManager::spawnAgents()
{
	agents.reserve(agentsAlive);
	for (int i = 0; i < agentsAlive; i++)
	{
		Agent a(levelData.randomWalkableLocation(), glm::radians((float)(rand() % 360)), i);
		agents.push_back(a);
	}
}

void AgentManager::hitAgentWithBullet(Agent& agent, Bullet& bullet)
{
	//Armour will absorb up to 50% of the bullet's damage
	float armourDamage = glm::min<float>(agent.currentArmour, bullet.damage * 0.5f);
	float healthDamage = bullet.damage - armourDamage;
	agent.currentArmour -= armourDamage;
	agent.currentHealth -= healthDamage;
	if (agent.currentHealth <= 0)
	{
		printf("Agent %i killed by agent %i. Survived %.1f seconds and had %i kills.\n", agent.id, bullet.shooter->id, agent.survivalTime, agent.killCount);
		bullet.shooter->killCount++;
		killAgent(agent);
	}
	bullet.hitTarget = true;
}

void AgentManager::hurtAgentsOutsideCircle(const Game& gameState)
{
	for (auto& agent : agents)
	{
		if (agent.activeAndAlive() && glm::length(agent.position - gameState.circleCentre) > gameState.circleRadius)
		{
			agent.currentHealth -= (int)gameState.circleDamageTick;
			if (agent.currentHealth < 0.0f)
			{
				printf("Agent %i died outside the circle.\n", agent.id);
				killAgent(agent);
			}
		}
	}
}

void AgentManager::killAgent(Agent& agent)
{
	if (agent.alive)	//sanity check
	{
		agent.currentHealth = 0;
		agent.alive = false;
		agentsAlive--;
		if (agentsAlive != 1)
		{
			printf("%i agents remain.\n", agentsAlive);
		}
		else
		{
			//find the only agent that is still alive. Guaranteed to be one, so we don't need to check the output
			Agent& lastSurvivor = *(std::find_if(agents.begin(), agents.end(), [](Agent& a) { return a.alive; }));
			printf("Agent %i wins after %.1f seconds, with %i kills.\n", lastSurvivor.id, lastSurvivor.survivalTime, lastSurvivor.killCount);
		}
	}
}

void AgentManager::cancelAllAgentTargets()
{
	for (auto& agent : agents)
	{
		agent.currentTargetType = TargetType::none;
	}
}

//Track which other surviving agents can be seen by a given agent
void AgentManager::updateAgentSightOfOtherAgents(Agent& agent)
{
	agent.otherVisibleAgents.clear();
	for (auto& otherAgent : agents)
	{
		//for now, visibility is just a simple distance check
		if (&otherAgent != &agent && otherAgent.activeAndAlive())
		{
			if (glm::length(agent.position - otherAgent.position) < AGENT_VISIBILITY_RANGE)
			{
				//printf("Agent %i sees agent %i\n", agent.id, otherAgent.id);
				agent.otherVisibleAgents.push_back(otherAgent);
			}
		}
	}
}

void AgentManager::updateAgentSightOfItems(Agent& agent, const Game& gameState)
{
	agent.visibleItems.clear();
	for (auto& item : gameState.items)
	{
		if (item.enabled)
		{
			if (glm::length(agent.position - item.position) < AGENT_VISIBILITY_RANGE)
			{
				agent.visibleItems.push_back(item);
			}
		}
	}
}

void AgentManager::restart()
{
	elapsedDamageTickTime = 0.0f;
	agentsAlive = MAX_AGENTS;
	for (auto& agent : agents)
	{
		agent.reset();
		agent.position = levelData.randomWalkableLocation();
	}
	bullets.clear();
}