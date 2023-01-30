#pragma once

#include <algorithm>
#include <vector>
#include "Agent.h"
#include "Bullet.h"
#include "Game.h"
#include "Level.h"
#include "glm/glm.hpp"

class AgentManager
{
public:
	AgentManager(int agentCount, Level& level);
	~AgentManager();
	std::vector<Agent> agents;
	void updateAgents(float frameTime, const Game& gameState);
	void updateBullets(float frameTime);
	void cancelAllAgentTargets();
	int agentsAlive;
	std::vector<Bullet> bullets;
	float checkCollision(Bullet& bullet, Agent& agent, float frameTime);
	void spawnAgents();
private:
	void hurtAgentsOutsideCircle(const Game& gameState);
	void killAgent(Agent& agent);
	void updateAgentSightOfOtherAgents(Agent& agent);
	Level& levelData;
	float elapsedDamageTickTime;
};