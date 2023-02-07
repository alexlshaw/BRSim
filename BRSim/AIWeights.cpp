#include "AIWeights.h"

AIWeights::AIWeights()
	:healthItemPriority(1.0f),
	armourItemPriority(1.0f),
	swapWeaponPriorities
	{
		{0.0f, 0.8f, 0.9f, 1.0f},		//weights for first gun
		{0.0f, 0.0f, 0.6f, 0.75f},		//weights if using pistol
		{0.0f, -10.0f, 0.0f, 0.5f},		//weights if using assault rifle
		{0.0f, -10.0f, -10.0f, 0.0f}	//weights if using sniper rifle
	},
	itemNotInNextCirclePriorityModifier(0.5f),
	itemNotInCurrentCirclePriorityModifier(0.5f),
	itemDistancePriority(1.0f),
	fleeHealthThreshold(50.0f),
	stopFleeingCircleThreshold(360.0f),
	weaponPickupWhileFleeingPriority(0.1f),
	ignoreItemThreshold(0.1f)
{}