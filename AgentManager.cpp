#include "AgentManager.h"

AgentManager::AgentManager(int agentCount)
{
	//init global agent params
	agentMaxRotateSpeed = 90.0f;
	agentMaxSpeed = 10.0f;

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

void AgentManager::updateAgents(float frameTime)
{
	for (auto& agent : agents)
	{
		if (agent.alive)
		{
			//for now:
			//1. If there is another living agent in range, fight them
			bool fighting = false;
			for (auto& otherAgent : agents)
			{
				if (&otherAgent != &agent && otherAgent.alive)
				{
					if (glm::length(agent.pos - otherAgent.pos) < agent.range)
					{
						fighting = true;
						agentFight(agent, otherAgent, frameTime);
					}
				}
			}
			//2. If no nearby agent, just wander randomly
			if (!fighting)
			{
				//2a. randomly rotate
				float rot = (float)(rand() % (int)agentMaxRotateSpeed) * frameTime;
				if (rand() % 2 == 0)
				{
					agent.look += glm::radians(rot);
				}
				else
				{
					agent.look -= glm::radians(rot);
				}
				//2b. randomly move
				float mSpeed = (float)(rand() % (int)agentMaxSpeed) * frameTime;
				glm::vec2 forward = agent.forward() * mSpeed;
				agent.pos = agent.pos + forward;
			}
			
		}
	}
}



void AgentManager::agentFight(Agent& agent, Agent& other, float deltaTime)
{
	//either rotate towards the other agent, or attack it
	glm::vec2 target = other.pos - agent.pos;
	glm::vec2 fwd = agent.forward();
	float angleBetween = glm::acos(glm::dot(target, fwd) / glm::length(target));	
	//check which way we actually have to rotate
	float side = (fwd.x * target.y) - (fwd.y * target.x);

	//rotate to face target (if possible)
	float maxRotation = glm::radians(agentMaxRotateSpeed) * deltaTime;
	//printf("Fwd: %.2f, %.2f || tgt: %.2f, %.2f || Angle: %.2f\n", fwd.x, fwd.y, target.x, target.y, glm::degrees(angleBetween));
	if (angleBetween > maxRotation)
	{
		agent.look += maxRotation * (side < 0.0f ? -1.0f : 1.0f); //if forward vector is left of target vector -> rotate clockwise
	}
	else
	{
		//we're close enough to directly face them
		//so we turn that way and attack
		agent.look += angleBetween * (side < 0.0f ? -1.0f : 1.0f); //if forward vector is left of target vector -> rotate clockwise
		agentAttack(agent, other);
	}
}

void AgentManager::agentAttack(Agent& agent, Agent& other)
{
	//for now attacks just instakill targets
	other.alive = false;
}