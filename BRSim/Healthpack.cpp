#include "Healthpack.h"
#include "Agent.h"

Healthpack::Healthpack(std::string textureName)
	:Item(textureName, HEALTHPACK) {}

void Healthpack::onPickup(Agent& agent)
{
	agent.currentHealth = glm::min<float>(agent.currentHealth + HEALTHPACK_HEAL_AMOUNT, AGENT_MAX_HEALTH);
}