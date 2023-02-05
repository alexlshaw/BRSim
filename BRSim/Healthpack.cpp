#include "Healthpack.h"
#include "Agent.h"

Healthpack::Healthpack(std::string textureName)
	:Item(textureName) {}

void Healthpack::onPickup(Agent& agent)
{
	agent.currentHealth = glm::min<int>(agent.currentHealth + HEALTHPACK_HEAL_AMOUNT, AGENT_MAX_HEALTH);
}