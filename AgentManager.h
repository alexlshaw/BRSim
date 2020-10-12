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
	void updateAgents(float frameTime);
	void killAgentsOutsideCircle(glm::vec2 circleCentre, float circleRadius);
private:
	float agentMaxRotateSpeed; //in degrees/sec
	float agentMaxSpeed;	//in pixels/sec
	void agentFight(Agent& agent, Agent& other, float deltaTime);
	void agentAttack(Agent& agent, Agent& other);
};