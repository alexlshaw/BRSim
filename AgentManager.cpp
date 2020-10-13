#include "AgentManager.h"

AgentManager::AgentManager(int agentCount)
{
	agentsAlive = agentCount;
	//init agents
	for (int i = 0; i < agentCount; i++)
	{
		float x = (float)(rand() % 400) + 200.0f;
		float y = (float)(rand() % 400) + 200.0f;
		glm::vec2 pos = glm::vec2(x, y);
		Agent a = Agent(pos, glm::radians((float)(rand() % 360)));
		agents.push_back(a);
	}
}

AgentManager::~AgentManager() {}

void AgentManager::updateAgents(float frameTime, glm::vec2 nextCircleCentre, float nextCircleRadius)
{
	for (auto& agent : agents)
	{
		if (agent.alive)
		{
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
			//2. If no nearby agent, does it have a target?
			if (!fighting)
			{
				if (agent.hasTarget)
				{
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
					findTargetForAgent(agent, nextCircleCentre, nextCircleRadius);
				}
			}
		}
	}
}

void AgentManager::killAgentsOutsideCircle(glm::vec2 circleCentre, float circleRadius)
{
	for (auto& agent : agents)
	{
		if (glm::length(agent.pos - circleCentre) > circleRadius && agent.alive)
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
	//for now attacks just instakill targets
	killAgent(other);
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