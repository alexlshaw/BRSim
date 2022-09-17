#include "AgentManager.h"

AgentManager::AgentManager(int agentCount, Level& level) 
	: levelData(level)
{
	agentsAlive = agentCount;
}

AgentManager::~AgentManager() {}

void AgentManager::updateAgents(float frameTime, Game* gameState)
{
	killAgentsOutsideCircle(gameState);
	for (auto& agent : agents)
	{
		if (agent.alive)
		{
			//progress cooldowns
			agent.shotCooldownRemainingTime = glm::max<float>(0.0f, agent.shotCooldownRemainingTime - frameTime);

			//1. If there is another living agent in range, fight them
			bool fighting = false;
			for (auto& otherAgent : agents)
			{
				if (&otherAgent != &agent && otherAgent.alive)
				{
					if (glm::length(agent.pos - otherAgent.pos) < agent.range)
					{
						agent.hasTarget = false;	//cancel whatever it was doing before, it's fighting time
						fighting = true;
						agentFight(agent, otherAgent, frameTime);
					}
				}
			}
			//2. If no nearby agent, is its target destination satisfactory??
			if (!fighting)
			{
				if (agent.hasTarget)
				{
					//check if it's still good
					if (!gameState->isPositionInsideNextCircle(agent.targetPosition))
					{
						//if not, cancel it and find a new one
						agent.hasTarget = false;
						findTargetForAgent(agent, gameState->nextCircleCentre, gameState->nextCircleRadius);
					}

					//2a. Turn and move towards target
					if (agent.rotateTowards(agent.targetPosition, frameTime))
					{
						if (agent.moveTowards(agent.targetPosition, frameTime))
						{
							agent.hasTarget = false;	//if the agent reached its goal, it no longer has a target
						}
					}
					
				}
				else
				{
					//2b. Find a target
					findTargetForAgent(agent, gameState->nextCircleCentre, gameState->nextCircleRadius);
				}
			}
		}
	}
}

void AgentManager::updateBullets(float frameTime)
{
	//Seems like at the moment, agents might be being instakilled by their own bullets?
	for (auto& bullet : bullets)
	{
		float minHitT = 99999.0f;
		int hitAgentIdx = -1;
		for (unsigned int j = 0; j < agents.size(); j++)
		{
			if (agents[j].alive)
			{
				float t = checkCollision(bullet, agents[j], frameTime);
				if (t != 0.0f && t < minHitT)
				{
					minHitT = t;
					hitAgentIdx = j;
				}
			}
		}
		//if we ended up hitting an agent, kill it
		if (hitAgentIdx != -1)
		{
			printf("Agent %i hit by shot fired by agent %i\n", agents[hitAgentIdx].id, bullet.ownerID);
			killAgent(agents[hitAgentIdx]);
			bullet.hitTarget = true;
		}
		bullet.life -= frameTime;
		bullet.pos += bullet.dir * frameTime * BULLET_SPEED;
	}
	auto it = std::remove_if(bullets.begin(), bullets.end(), [](Bullet& x) { return x.life <= 0.0f || x.hitTarget; });
	bullets.erase(it, bullets.end());
}

//it seems bullets are behaving more like laser beams right now
float AgentManager::checkCollision(Bullet& bullet, Agent& agent, float frameTime)
{
	glm::vec2 endPos = bullet.pos + bullet.dir * frameTime * BULLET_SPEED;
	glm::vec2 trajectory = endPos - bullet.pos;
	glm::vec2 AC = agent.pos - bullet.pos;
	//project AC onto trajectory
	float pt = glm::dot(trajectory, AC) / glm::length(trajectory);
	//make sure the target is actually along the section of the trajectory line that the bullet will travel in this frame
	if (pt < 0.0f || pt > glm::length(trajectory))
	{
		return false;
	}
	glm::vec2 closestPoint = bullet.pos + pt * glm::normalize(trajectory);
	if (glm::length(agent.pos - closestPoint) < AGENT_COLLISION_RADIUS)
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

void AgentManager::spawnAgents()
{
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

void AgentManager::killAgentsOutsideCircle(Game* gameState)
{
	for (auto& agent : agents)
	{
		if (glm::length(agent.pos - gameState->circleCentre) > gameState->circleRadius && agent.alive)
		{
			killAgent(agent);
		}
	}
}

void AgentManager::agentFight(Agent& agent, Agent& other, float deltaTime)
{
	if (agent.rotateTowards(other.pos, deltaTime))
	{
		agentAttack(agent, other);
	}
}

void AgentManager::agentAttack(Agent& agent, Agent& other)
{
	if (agent.shotCooldownRemainingTime <= 0.0f)
	{
		bullets.push_back(Bullet(agent.pos + 3.0f * agent.forward(), agent.forward(), 2.0f, agent.id));
		agent.shotCooldownRemainingTime = AGENT_SHOT_COOLDOWN;
	}
	
}

void AgentManager::killAgent(Agent& agent)
{
	if (agent.alive)	//sanity check
	{
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

void AgentManager::findTargetForAgent(Agent& agent, glm::vec2 nextCircleCentre, float nextCircleRadius)
{
	float distanceToNextCircleCentre = glm::length(agent.pos - nextCircleCentre);
	if (distanceToNextCircleCentre > nextCircleRadius)
	{
		float targetDistance = distanceToNextCircleCentre - (0.95f * nextCircleRadius);
		agent.setTarget(agent.pos + glm::normalize(nextCircleCentre - agent.pos) * targetDistance);
	}
	else
	{
		//agent is in the next circle, just pick a random spot (within the next circle)
		float rTheta = glm::radians((float)(rand() % 360));
		float rRad = (float)(rand() % (int)(nextCircleRadius));
		float rx = glm::cos(rTheta) * rRad;
		float ry = glm::sin(rTheta) * rRad;
		glm::vec2 randomSpot = nextCircleCentre + glm::vec2(rx, ry);
		//make sure the spot in question is actually within the level
		randomSpot.x = glm::clamp(randomSpot.x, 0.0f, (float)levelData.width);
		randomSpot.y = glm::clamp(randomSpot.y, 0.0f, (float)levelData.height);
		agent.setTarget(randomSpot);
	}
}

void AgentManager::cancelAllAgentTargets()
{
	for (auto& agent : agents)
	{
		agent.hasTarget = false;
	}
}