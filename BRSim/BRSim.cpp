// BRSim.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <Windows.h>
#include <iostream>
#include <memory.h>
#include <thread>
#include <vector>
#include "AgentManager.h"
#include "Agent.h"
#include "Bullet.h"
#include "CStopWatch.h"
#include "Game.h"
#include "Level.h"
#include "Renderer.h"
#include "Settings.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "time.h"

//Window control variables
GLFWwindow* mainWindow = nullptr;
bool shouldExit = false;
int screenWidth = 1024, screenHeight = 1024;

//variables for manipulating viewport and simulation
bool panning = false;
glm::vec2 mouseClickStartLoc;
float timeRate = 1.0f;

std::unique_ptr<Renderer> renderer;
std::unique_ptr<Level> level;
std::unique_ptr<AgentManager> manager;
std::unique_ptr<Game> gameState;

void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		shouldExit = true;
	}
	if (key == GLFW_KEY_T && action == GLFW_PRESS)
	{
		renderer->toggleShowTargetingLines();
	}
	if (key == GLFW_KEY_W && action == GLFW_PRESS)
	{
		renderer->toggleShowLevelWalkData();
	}
	if (key == GLFW_KEY_H && action == GLFW_PRESS)
	{
		renderer->toggleShowHealthBars();
	}
	if (key == GLFW_KEY_O && action == GLFW_PRESS)
	{
		//reduce timestep
		timeRate -= 1.0f;
		printf("Decreasing time scale to %i\n", (int)timeRate);
	}
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
	{
		//increase timestep
		timeRate += 1.0f;
		printf("Increasing time scale to %i\n", (int)timeRate);
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		mouseClickStartLoc = glm::vec2(xpos, ypos);
		renderer->windowOffsetAtPanStart = renderer->windowOffset;
		panning = true;
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
	{
		panning = false;
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	//mousewheel only gives y offsets (integral values, 1 per mouse-wheel-click per frame it seems, up is +ve, down -ve)
	if (yoffset > 0.0f)
	{
		renderer->zoomLevel *= 2.0f;
	}
	else if (yoffset < 0.0f)
	{
		renderer->zoomLevel *= 0.5f;
	}
	printf("Zooming to: %.2f\n", renderer->zoomLevel);
}

int init_GLFW()
{
	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
	{
		printf("GLFW init error\n");
		return -1;
	}
	//Create a windowed mode window and its OpenGL context. Note: If I hint anything over 3.0 it doesn't like it, but testing shows I'm getting a 4.6 context
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	mainWindow = glfwCreateWindow(screenWidth, screenHeight, "BRSim", NULL, NULL);
	if (!mainWindow)
	{
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(mainWindow);
	//input handlers
	glfwSetKeyCallback(mainWindow, key_callback);
	glfwSetMouseButtonCallback(mainWindow, mouse_button_callback);
	glfwSetScrollCallback(mainWindow, scroll_callback);
	//glfwSetCursorPosCallback(mainWindow, cursor_position_callback);
	//set up glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		printf("GLAD loader initialization error!\n");
		return -1;
	}
	glfwSwapInterval(1);
	return 1;
}

void update(float frameTime)
{
	//update moust control of screen position
	if (panning)
	{
		double xpos, ypos;
		glfwGetCursorPos(mainWindow, &xpos, &ypos);
		glm::vec2 mouseEndLoc = glm::vec2(xpos, ypos);
		glm::vec2 delta = (mouseEndLoc - mouseClickStartLoc) / renderer->zoomLevel;
		renderer->windowOffset = renderer->windowOffsetAtPanStart + glm::vec2(-delta.x, delta.y);
	}
	//update simulation state
	if (manager->agentsAlive > 1)
	{
		manager->updateBullets(frameTime);
		manager->updateAgents(frameTime, *gameState);
		gameState->update(frameTime);
	}
}

void exit()
{
	glfwTerminate();
}

void generateData()
{
	int startTime = (int)time(NULL);
	level = std::make_unique<Level>(2048, 2048, "sanhok");
	int levelTime = (int)time(NULL);
	printf("Level loaded in %i seconds\n", levelTime - startTime);
	manager = std::make_unique<AgentManager>(MAX_AGENTS, *level);
	manager->spawnAgents();
	printf("Agents loaded in %i seconds\n", ((int)time(NULL) - levelTime));
	gameState = std::make_unique<Game>(*level);
	int totalTime = (int)time(NULL) - startTime;
	printf("Total time: %i\n", totalTime);
}

int main()
{
	static CStopWatch timer;
	if (!init_GLFW())
	{
		return -1;
	}
	srand((unsigned int)(time(NULL)));
	generateData();
	renderer = std::make_unique<Renderer>(mainWindow, *level);
	timer.Reset();
	/* Loop until the user closes the window */
	while (!shouldExit && !glfwWindowShouldClose(mainWindow))
	{
		float delta = timer.GetElapsedSeconds();
		timer.Reset();
		update(delta * timeRate);
		renderer->draw(*gameState, *level, *manager);
		glfwPollEvents();
		delta = timer.GetElapsedSeconds() * 1000.0f;
		if (delta < 16.6f)	//cap framerate at ~60fps
		{
			long sleep = (long)(16.6f - delta);
			if (sleep > 0l)	//0L, not 01
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(sleep));
			}
		}
	}
	exit();
	return 0;
}