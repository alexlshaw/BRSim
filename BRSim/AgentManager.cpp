#include "AgentManager.h"

AgentManager::AgentManager(int agentCount, Level& level) 
	: levelData(level)
{
	agentsAlive = agentCount;
}

AgentManager::~AgentManager() {}

void AgentManager::updateAgents(float frameTime, const Game& gameState)
{
	killAgentsOutsideCircle(gameState);
	for (auto& agent : agents)
	{
		if (agent.alive)
		{
			updateAgentSightOfOtherAgents(agent);
			agent.update(frameTime, gameState);
			if (agent.firing)
			{
				bullets.push_back(Bullet(agent.pos + 3.0f * agent.forward(), agent.forward(), 2.0f, agent.id));
				agent.shotCooldownRemainingTime = AGENT_SHOT_COOLDOWN;
				agent.firing = false;
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

void AgentManager::killAgentsOutsideCircle(const Game& gameState)
{
	for (auto& agent : agents)
	{
		if (glm::length(agent.pos - gameState.circleCentre) > gameState.circleRadius && agent.alive)
		{
			killAgent(agent);
		}
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
		if (&otherAgent != &agent && otherAgent.alive)
		{
			if (glm::length(agent.pos - otherAgent.pos) < agent.range)
			{
				agent.otherVisibleAgents.push_back(otherAgent);
			}
		}
	}
}