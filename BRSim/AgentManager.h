#pragma once

#include <vector>
#include "Agent.h"
#include "Bullet.h"
#include "glm/glm.hpp"
#include "glm\gtx\transform.hpp" 

class AgentManager
{
public:
	AgentManager(int agentCount);
	~AgentManager();
	std::vector<Agent> agents;
	void updateAgents(float frameTime, glm::vec2 nextCircleCentre, float nextCircleRadius);
	void updateBullets(float frameTime);
	void killAgentsOutsideCircle(glm::vec2 circleCentre, float circleRadius);
	void cancelAllAgentTargets();
	int agentsAlive;
	std::vector<Bullet> bullets;
	float checkCollision(Bullet& bullet, Agent& agent, float frameTime);
private:
	void agentFight(Agent& agent, Agent& other, float deltaTime);
	void agentAttack(Agent& agent, Agent& other);
	void killAgent(Agent& agent);
	void findTargetForAgent(Agent& agent, glm::vec2 nextCircleCentre, float nextCircleRadius);
};