// BRSim.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtx\transform.hpp" 
#include "glm\gtx\compatibility.hpp"
#include "time.h"
#include <iostream>
#include <thread>
#include <vector>
#include "AgentManager.h"
#include "Agent.h"
#include "Bullet.h"
#include "CStopWatch.h"
#include "Level.h"
#include "Mesh.h"
#include "Settings.h"
#include "Shader.h"
#include "Vertex.h"

//Window control variables
GLFWwindow* mainWindow = nullptr;
bool shouldExit = false;
int screenWidth = 1024, screenHeight = 1024;

//variables for manipulating viewport and simulation
glm::vec2 windowOffset = glm::vec2(0.0f, 0.0f);
glm::vec2 windowOffsetAtPanStart = windowOffset;
bool panning = false;
glm::vec2 mouseClickStartLoc;
float zoomLevel = 1.0f;
float timeRate = 1.0f;

//Variables for the basic graphics stuff
Shader* basic = nullptr;
Shader* texturedUnlit = nullptr;
Shader* agentShader = nullptr;
int uBProjMatrix, uBModelMatrix, uTProjMatrix, uTModelMatrix, uTex;
int uAProj, uAModel;
GLuint avbo, avao, aibo;
GLuint tvbo, tvao, tibo;

Level* level = nullptr;
bool showLevelWalkData = false;
AgentManager* manager = nullptr;
Mesh agentMesh, agentTargetingCircleMesh;

Mesh lineMesh;
std::vector<Vertex> linePoints;
bool showTargetingLines = false;

//circle of death properties
Mesh circleOfDeathMesh, nextCircleMesh;
glm::vec2 circleCentre, nextCircleCentre, previousCircleCentre;
float circleRadius, previousCircleRadius, nextCircleRadius;
float elapsedShrinkTime = 0.0f;

void buildMeshes()
{
	std::vector<Vertex> vertices;
	vertices.push_back(Vertex(glm::vec4(16.0f, 0.0f, 0.0f, 1.0f), black));
	vertices.push_back(Vertex(glm::vec4(0.0f, 4.0f, 0.0f, 1.0f), black));
	vertices.push_back(Vertex(glm::vec4(0.0f, -4.0f, 0.0f, 1.0f), black));
	std::vector<unsigned int> indices;
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);

	agentMesh.Load(vertices, indices);

	//now build the agent awareness/targeting circle mesh
	vertices.clear();
	indices.clear();
	int sides = 32;
	for (int i = 0; i < sides; i++)
	{
		float angle = glm::radians((360.0f / (float)sides) * (float)i);
		glm::vec4 p = glm::vec4(glm::cos(angle), glm::sin(angle), 0.0f, 1.0f);
		vertices.push_back(Vertex(p, red));
		indices.push_back(i);
	}
	agentTargetingCircleMesh.Load(vertices, indices);

	//now we build the circle of death mesh
	vertices.clear();
	indices.clear();
	for (int i = 0; i < sides; i++)
	{
		float angle1 = glm::radians((360.0f / (float)sides) * (float)i);
		float angle2 = glm::radians((360.0f / (float)sides) * (float)(i + 1));
		glm::vec4 p1 = glm::vec4(glm::cos(angle1), glm::sin(angle1), 0.0f, 1.0f);
		glm::vec4 p2 = glm::vec4(glm::cos(angle2), glm::sin(angle2), 0.0f, 1.0f);
		glm::vec4 p3 = glm::vec4(p1.x * 50.0f, p1.y * 50.0f, 0.0f, 1.0f);
		glm::vec4 p4 = glm::vec4(p2.x * 50.0f, p2.y * 50.0f, 0.0f, 1.0f);
		vertices.push_back(Vertex(p1, translucentBlue));	//i * 4
		vertices.push_back(Vertex(p2, translucentBlue));
		vertices.push_back(Vertex(p3, translucentBlue));	
		vertices.push_back(Vertex(p4, translucentBlue));
		//tri 1 (p1->p4->p2)
		indices.push_back(i * 4);
		indices.push_back(i * 4 + 3);
		indices.push_back(i * 4 + 1);
		//tri 2 (p1->p3->p4)
		indices.push_back(i * 4);
		indices.push_back(i * 4 + 2);
		indices.push_back(i * 4 + 3);
	}
	circleOfDeathMesh.Load(vertices, indices);

	//now we build the nextCircle mesh (same as targeting circle but in blue, need to make this generic)
	vertices.clear();
	indices.clear();
	for (int i = 0; i < sides; i++)
	{
		float angle = glm::radians((360.0f / (float)sides) * (float)i);
		glm::vec4 p = glm::vec4(glm::cos(angle), glm::sin(angle), 0.0f, 1.0f);
		vertices.push_back(Vertex(p, translucentBlue));
		indices.push_back(i);
	}
	nextCircleMesh.Load(vertices, indices);
}

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
		showTargetingLines = !showTargetingLines;
	}
	if (key == GLFW_KEY_W && action == GLFW_PRESS)
	{
		showLevelWalkData = !showLevelWalkData;
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
		windowOffsetAtPanStart = windowOffset;
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
		zoomLevel *= 2.0f;
	}
	else if (yoffset < 0.0f)
	{
		zoomLevel *= 0.5f;
	}
	printf("Zooming to: %.2f\n", zoomLevel);
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

void init_GL()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glActiveTexture(GL_TEXTURE0);
	if (glGetError() != GL_NO_ERROR)
	{
		printf("GL Initialisation error\n");
	}
	//load shaders
	basic = new Shader();
	basic->compileShaderFromFile("./Data/Shaders/basic.vert", VERTEX);
	basic->compileShaderFromFile("./Data/Shaders/basic.frag", FRAGMENT);
	basic->linkAndValidate();
	uBProjMatrix = basic->getUniformLocation("projectionViewMatrix");
	uBModelMatrix = basic->getUniformLocation("modelMatrix");
	texturedUnlit = new Shader();
	texturedUnlit->compileShaderFromFile("./Data/Shaders/TexturedUnlit.vert", VERTEX);
	texturedUnlit->compileShaderFromFile("./Data/Shaders/TexturedUnlit.frag", FRAGMENT);
	texturedUnlit->linkAndValidate();
	uTProjMatrix = texturedUnlit->getUniformLocation("projectionViewMatrix");
	uTModelMatrix = texturedUnlit->getUniformLocation("modelMatrix");
	uTex = texturedUnlit->getUniformLocation("tex");

	int width, height;
	glfwGetFramebufferSize(mainWindow, &width, &height);
	glViewport(0, 0, width, height);
}

void newCircle()
{
	nextCircleRadius = circleRadius / 2.0f;
	//compute random bearing and distance from current circle centre
	float angle = glm::radians((float)(rand() % 360));
	float distance = (float)(rand() % (int)nextCircleRadius);
	nextCircleCentre = circleCentre + glm::vec2(glm::cos(angle) * distance, glm::sin(angle) * distance);
	elapsedShrinkTime = 0.0f;
	manager->cancelAllAgentTargets();
}

void update(float frameTime)
{
	//update moust control of screen position
	if (panning)
	{
		double xpos, ypos;
		glfwGetCursorPos(mainWindow, &xpos, &ypos);
		glm::vec2 mouseEndLoc = glm::vec2(xpos, ypos);
		glm::vec2 delta = (mouseEndLoc - mouseClickStartLoc) / zoomLevel;
		windowOffset = windowOffsetAtPanStart + glm::vec2(-delta.x, delta.y);
	}
	//update simulation state
	if (manager->agentsAlive > 1)
	{
		manager->updateBullets(frameTime);
		manager->updateAgents(frameTime, nextCircleCentre, nextCircleRadius);
		manager->killAgentsOutsideCircle(circleCentre, circleRadius);
		elapsedShrinkTime += frameTime;
		float t = elapsedShrinkTime / CIRCLE_SHRINK_TIME;
		//if we're finished shrinking this circle
		if (t >= 1.0f)
		{
			circleRadius = nextCircleRadius;
			circleCentre = nextCircleCentre;
			previousCircleRadius = circleRadius;
			previousCircleCentre = circleCentre;
			newCircle();
		}
		else
		{
			circleRadius = glm::lerp(previousCircleRadius, nextCircleRadius, t);
			circleCentre = glm::lerp(previousCircleCentre, nextCircleCentre, t);
		}
	}
}

//this function doesn't actually draw a line, it just adds the line data to the batch to be drawn later
void drawLine(glm::vec2 start, glm::vec2 end, glm::vec4 colour)
{
	linePoints.push_back(Vertex(glm::vec4(start.x, start.y, 0.0f, 1.0f), colour));
	linePoints.push_back(Vertex(glm::vec4(end.x, end.y, 0.0f, 1.0f), colour));
}

//this function takes the accumulated line data, and sends it off to the GPU to be drawn
void drawLines()
{
	if (linePoints.size() > 0)
	{
		std::vector<unsigned int> indices;
		for (unsigned int i = 0; i < linePoints.size(); i++)
		{
			indices.push_back(i);
		}
		lineMesh.Load(linePoints, indices);
		basic->setUniform(uBModelMatrix, glm::identity<glm::mat4>());
		lineMesh.draw(GL_LINES);
	}
}

glm::mat4 computeProjection()
{
	float viewWidth = screenWidth / (zoomLevel * 2.0f);
	float viewHeight = screenHeight / (zoomLevel * 2.0f);

	float centreX = (screenWidth / 2.0f) + windowOffset.x;
	float centreY = (screenHeight / 2.0f) + windowOffset.y;

	float left = centreX - viewWidth;
	float right = centreX + viewWidth;
	float bottom = centreY - viewHeight;
	float top = centreY + viewHeight;
	glm::mat4 projection = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
	return projection;
}

void draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	linePoints.clear();
	glm::mat4 projection = computeProjection();

	//first draw the level
	texturedUnlit->use();
	texturedUnlit->setUniform(uTProjMatrix, projection);
	//texturedUnlit->setUniform(uTModelMatrix, glm::identity<mat4>());
	texturedUnlit->setUniform(uTex, 0);
	level->draw(texturedUnlit, uTModelMatrix, showLevelWalkData);

	basic->use();
	basic->setUniform(uBProjMatrix, projection);

	for (auto& agent : manager->agents)
	{
		glm::mat4 tr = glm::translate(glm::vec3(agent.pos.x, agent.pos.y, 0.0f));
		glm::mat4 rot = glm::rotate(agent.look, glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 modelview = tr * rot;
		basic->setUniform(uBModelMatrix, modelview);
		if (agent.alive)
		{
			agentMesh.draw();
		}
		else
		{
			agentMesh.draw(GL_LINE_LOOP);
		}
	}
	//draw targeting circles
	for (auto& agent : manager->agents)
	{
		glm::mat4 tr = glm::translate(glm::vec3(agent.pos.x, agent.pos.y, 0.0f));
		glm::mat4 sc = glm::scale(glm::vec3(agent.range, agent.range, agent.range));
		glm::mat4 modelview = tr * sc;
		basic->setUniform(uBModelMatrix, modelview);
		if (agent.alive)
		{
			agentTargetingCircleMesh.draw(GL_LINE_LOOP);
		}
	}

	//draw bullets
	for (auto& bullet : manager->bullets)
	{
		drawLine(bullet.pos, bullet.pos + (bullet.dir * 2.0f), black);
	}

	//draw target lines
	if (showTargetingLines)
	{
		for (auto& agent : manager->agents)
		{
			if (agent.hasTarget)
			{
				drawLine(agent.pos, agent.targetPosition, green);
			}
		}
	}

	//draw circle of death
	glm::mat4 tr = glm::translate(glm::vec3(circleCentre.x, circleCentre.y, 0.0f));
	glm::mat4 sc = glm::scale(glm::vec3(circleRadius, circleRadius, circleRadius));
	glm::mat4 modelview = tr * sc;
	basic->setUniform(uBModelMatrix, modelview);
	circleOfDeathMesh.draw();

	//draw upcoming circle
	tr = glm::translate(glm::vec3(nextCircleCentre.x, nextCircleCentre.y, 0.0f));
	sc = glm::scale(glm::vec3(nextCircleRadius, nextCircleRadius, nextCircleRadius));
	modelview = tr * sc;
	basic->setUniform(uBModelMatrix, modelview);
	nextCircleMesh.draw(GL_LINE_LOOP);

	//draw level boundary
	drawLine(glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, level->height), black);
	drawLine(glm::vec2(0.0f, 0.0f), glm::vec2(level->width, 0.0f), black);
	drawLine(glm::vec2(level->width, level->height), glm::vec2(0.0f, level->height), black);
	drawLine(glm::vec2(level->width, level->height), glm::vec2(level->width, 0.0f), black);

	drawLines();


	glfwSwapBuffers(mainWindow);
}

void exit()
{
	if (basic != nullptr)
	{
		delete basic;
		basic = nullptr;
	}
	if (texturedUnlit != nullptr)
	{
		delete texturedUnlit;
		texturedUnlit = nullptr;
	}
	if (manager != nullptr)
	{
		delete manager;
		manager = nullptr;
	}
	if (level != nullptr)
	{
		delete level;
		level = nullptr;
	}
	glfwTerminate();
}

void generateData()
{
	int startTime = (int)time(NULL);
	level = new Level(2048, 2048, "sanhok");
	int levelTime = (int)time(NULL);
	printf("Level loaded in %i seconds\n", levelTime - startTime);
	manager = new AgentManager(MAX_AGENTS, *level);
	manager->spawnAgents();
	printf("Agents loaded in %i seconds\n", ((int)time(NULL) - levelTime));
	circleCentre = glm::vec2(level->width / 2.0f, level->height / 2.0f);
	circleRadius = glm::max<float>(level->width, level->height) * 1.5f;
	previousCircleRadius = circleRadius;
	previousCircleCentre = circleCentre;
	newCircle();
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
	init_GL();
	srand((unsigned int)(time(NULL)));
	generateData();
	buildMeshes();
	timer.Reset();
	/* Loop until the user closes the window */
	while (!shouldExit && !glfwWindowShouldClose(mainWindow))
	{
		float delta = timer.GetElapsedSeconds();
		timer.Reset();
		update(delta * timeRate);
		draw();
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