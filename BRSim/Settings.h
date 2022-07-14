#pragma once

#include "glm\glm.hpp"

const int MAX_AGENTS = 20;					//default 20
const float AGENT_DEFAULT_RANGE = 60.0f;	//default 30
//const float INITIAL_CIRCLE_RADIUS = 800.0f;	//default 800;
const float CIRCLE_SHRINK_TIME = 20.0f;		//default 20

const float AGENT_MAX_ROTATE_SPEED = 90.0f;
const float AGENT_MAX_SPEED = 20.0f;
const float AGENT_SHOT_COOLDOWN = 1.0f;

const float BULLET_SPEED = 100.0f;	//default 100
const float AGENT_COLLISION_RADIUS = 1.0f;

//colours
const glm::vec4 white = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
const glm::vec4 black = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
const glm::vec4 red = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
const glm::vec4 translucentBlue = glm::vec4(0.0f, 0.0f, 1.0f, 0.35f);
const glm::vec4 green = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);

//level stuff
const bool LOAD_LEVEL_SURFACE_DATA = true;