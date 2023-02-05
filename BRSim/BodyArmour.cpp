#include "BodyArmour.h"
#include "Agent.h"

BodyArmour::BodyArmour(std::string textureName)
	:Item(textureName, BODYARMOUR) {}

void BodyArmour::onPickup(Agent& agent)
{
	agent.currentArmour = glm::min<float>(agent.currentArmour + BODYARMOUR_ARMOUR_AMOUNT, AGENT_MAX_ARMOUR);
}