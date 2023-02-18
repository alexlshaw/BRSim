#pragma once

//This class is basically a wrapper for a whole bunch of variables that are used in the AI decision making process
//Each Agent gets an instance of this, which can have unique values
class AIWeights
{
public:
	AIWeights();	//default ctor just gives default weights for everything
	float healthItemPriority;
	float armourItemPriority;
	float swapWeaponPriorities[4][4];	//[current weapon][available weapon]. Indices correspond to the WeaponType array in EquippedWeapon.h
	float itemNotInNextCirclePriorityModifier;
	float itemNotInCurrentCirclePriorityModifier;
	float itemDistancePriority;
	float fleeHealthThreshold;	//Health value (not fraction) at which the agent will flee from a fight
	float stopFleeingCircleThreshold;	//Radius of the circle at which the agent will stop running and fight regardless of their HP
	float weaponPickupWhileFleeingPriority;
	float ignoreItemThreshold;
};