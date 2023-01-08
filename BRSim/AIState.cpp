#include "AIState.h"
#include "Agent.h"	//Agent needs to be included here as using forward declaration

void AIState::setAgentState(Agent& owner, AIState* newState)
{
	AIState* oldState = owner.currentState;
	owner.currentState = newState;
	delete oldState;
}