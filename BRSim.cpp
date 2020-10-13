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
#include "CStopWatch.h"
#include "Mesh.h"
#include "Settings.h"
#include "Shader.h"
#include "Vertex.h"

GLFWwindow* mainWindow = nullptr;
bool shouldExit = false;
int screenWidth = 1024, screenHeight = 1024;

Shader* basic = nullptr;
Shader* texturedUnlit = nullptr;
Shader* agentShader = nullptr;
int uBProjMatrix, uBModelMatrix, uTProjMatrix, uTModelMatrix, uTex;
int uAProj, uAModel;
GLuint avbo, avao, aibo;
GLuint tvbo, tvao, tibo;

AgentManager* manager = nullptr;
Mesh agentMesh, agentTargetingCircleMesh;

Mesh lineMesh;
std::vector<glm::vec2> linePoints;
bool showTargetingLines = false;

//circle of death properties
Mesh circleOfDeathMesh, nextCircleMesh;
glm::vec2 circleCentre = glm::vec2(512.0f, 512.0f);
float circleRadius = INITIAL_CIRCLE_RADIUS;
float previousCircleRadius = circleRadius;
float nextCircleRadius;
glm::vec2 nextCircleCentre;
glm::vec2 previousCircleCentre = circleCentre;
float elapsedShrinkTime = 0.0f;

void buildMeshes()
{
	glm::vec4 white = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec4 black = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 red = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 blue = glm::vec4(0.0f, 0.0f, 1.0f, 0.35f);
	glm::vec4 green = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
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
		vertices.push_back(Vertex(p1, blue));	//i * 4
		vertices.push_back(Vertex(p2, blue));
		vertices.push_back(Vertex(p3, blue));	
		vertices.push_back(Vertex(p4, blue));
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
		vertices.push_back(Vertex(p, blue));
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
	if (manager->agentsAlive > 1)
	{
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
void drawLine(glm::vec2 start, glm::vec2 end)
{
	linePoints.push_back(start);
	linePoints.push_back(end);
}

//this function takes the accumulated line data, and sends it off to the GPU to be drawn
void drawLines()
{
	if (linePoints.size() > 0)
	{
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		for (unsigned int i = 0; i < linePoints.size(); i++)
		{
			vertices.push_back(Vertex(glm::vec4(linePoints[i].x, linePoints[i].y, 0.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)));
			indices.push_back(i);
		}
		lineMesh.Load(vertices, indices);
		basic->setUniform(uBModelMatrix, glm::identity<glm::mat4>());
		lineMesh.draw(GL_LINES);
	}
}

void draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	linePoints.clear();
	glm::mat4 projection = glm::ortho(0.0f, (float)screenWidth, 0.0f, (float)screenHeight, -1.0f, 1.0f);

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

	//draw target lines
	if (showTargetingLines)
	{
		for (auto& agent : manager->agents)
		{
			if (agent.hasTarget)
			{
				drawLine(agent.pos, agent.targetPosition);
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
	glfwTerminate();
}

void generateData()
{
	int sTime = (int)time(NULL);
	manager = new AgentManager(MAX_AGENTS);
	newCircle();
	int tTime = (int)time(NULL) - sTime;
	printf("Generation time: %i\n", tTime);
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
		update(delta);
		draw();
		glfwPollEvents();
		delta = timer.GetElapsedSeconds() * 1000.0f;
		if (delta < 16.6f)	//cap framerate at ~60fps
		{
			long sleep = (long)(16.6f - delta);
			std::this_thread::sleep_for(std::chrono::milliseconds(sleep));
		}
	}
	exit();
	return 0;
}