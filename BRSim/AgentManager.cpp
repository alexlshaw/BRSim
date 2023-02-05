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
			if (agent.firing)
			{
				bullets.push_back(Bullet(agent.position + 3.0f * agent.forward(), agent.forward(), 2.0f, agent.id));
				agent.shotCooldownRemainingTime = AGENT_SHOT_COOLDOWN;
				agent.firing = false;
			}
		}
	}
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
	glm::vec2 endPos = bullet.position + bullet.direction * frameTime * BULLET_SPEED;
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
		if (bullet.ownerID == agent.id)
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
		if (item.enabled && glm::length(agent.position - item.position) < ITEM_COLLISION_RADIUS)
		{
			item.onPickup(agent);
		}
	}
}

void AgentManager::spawnAgents()
{
	agents.reserve(agentsAlive);
	for (int i = 0; i < agentsAlive; i++)
	{
		bool valid = false;
		glm::vec2 pos = glm::vec2();
		while (!valid)
		{
			float x = (float)(rand() % (int)levelData.width);
			float y = (float)(rand() % (int)levelData.height);
			pos = glm::vec2(x, y);
			valid = levelData.getLevelInfo(x, y).walkable;
		}
		Agent a = Agent(pos, glm::radians((float)(rand() % 360)), i);
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
		killAgent(agent);
		printf("Agent %i killed by agent %i.\n", agent.id, bullet.ownerID);
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
			printf("%i agent remains.\n", agentsAlive);
		}
	}
}

void AgentManager::cancelAllAgentTargets()
{
	for (auto& agent : agents)
	{
		agent.hasTarget = false;
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