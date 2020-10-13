#pragma once

#include <vector>
#include "Agent.h"
#include "glm/glm.hpp"
#include "glm\gtx\transform.hpp" 

class AgentManager
{
public:
	AgentManager(int agentCount);
	~AgentManager();
	std::vector<Agent> agents;
	void updateAgents(float frameTime, glm::vec2 nextCircleCentre, float nextCircleRadius);
	void killAgentsOutsideCircle(glm::vec2 circleCentre, float circleRadius);
	void cancelAllAgentTargets();
	int agentsAlive;
private:
	void agentFight(Agent& agent, Agent& other, float deltaTime);
	void agentAttack(Agent& agent, Agent& other);
	void killAgent(Agent& agent);
	void findTargetForAgent(Agent& agent, glm::vec2 nextCircleCentre, float nextCircleRadius);
};