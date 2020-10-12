// BRSim.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtx\transform.hpp" 
#include "time.h"
#include <iostream>
#include <thread>
#include <vector>
#include "AgentManager.h"
#include "Agent.h"
#include "CStopWatch.h"
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


void buildAgentMesh()
{
	glm::vec4 white = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec4 black = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 red = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	std::vector<Vertex> vertices;
	vertices.push_back(Vertex(glm::vec4(16.0f, 0.0f, 0.0f, 1.0f), black));
	vertices.push_back(Vertex(glm::vec4(0.0f, 4.0f, 0.0f, 1.0f), black));
	vertices.push_back(Vertex(glm::vec4(0.0f, -4.0f, 0.0f, 1.0f), black));
	std::vector<unsigned int> indices;
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(0);

	//seems a bit overkill but whatever
	glGenVertexArrays(1, &avao);
	glGenBuffers(1, &avbo);
	glBindVertexArray(avao);
	glBindBuffer(GL_ARRAY_BUFFER, avbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
	//position
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)0);
	glEnableVertexAttribArray(0);
	//color
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)16);
	glEnableVertexAttribArray(1);
	glGenBuffers(1, &aibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, aibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indices.size(), &indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//now build the agent awareness/targeting circle mesh
	vertices.clear();
	indices.clear();
	int sides = 16;
	for (int i = 0; i < sides; i++)
	{
		float angle = glm::radians((360.0f / (float)sides) * (float)i);
		glm::vec4 p = glm::vec4(glm::cos(angle), glm::sin(angle), 0.0f, 1.0f);
		vertices.push_back(Vertex(p, red));
		indices.push_back(i);
	}
	indices.push_back(0);
	glGenVertexArrays(1, &tvao);
	glGenBuffers(1, &tvbo);
	glBindVertexArray(tvao);
	glBindBuffer(GL_ARRAY_BUFFER, tvbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
	//position
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)0);
	glEnableVertexAttribArray(0);
	//color
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)16);
	glEnableVertexAttribArray(1);
	glGenBuffers(1, &tibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indices.size(), &indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
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

void update(float frameTime)
{
	manager->updateAgents(frameTime);
}

void draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 projection = glm::ortho(0.0f, (float)screenWidth, 0.0f, (float)screenHeight, -1.0f, 1.0f);
	//glm::mat4 modelview = glm::identity<glm::mat4>();
	
	basic->use();
	basic->setUniform(uBProjMatrix, projection);

	glBindVertexArray(avao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, aibo);
	for (auto& agent : manager->agents)
	{
		glm::mat4 tr = glm::translate(glm::vec3(agent.pos.x, agent.pos.y, 0.0f));
		glm::mat4 rot = glm::rotate(agent.look, glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 modelview = tr * rot;
		basic->setUniform(uBModelMatrix, modelview);
		if (agent.alive)
		{
			glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)0);
		}
		else
		{
			glDrawElements(GL_LINE_STRIP, 4, GL_UNSIGNED_INT, (void*)0);
		}
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	//draw targeting circles
	glBindVertexArray(tvao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tibo);
	for (auto& agent : manager->agents)
	{
		glm::mat4 tr = glm::translate(glm::vec3(agent.pos.x, agent.pos.y, 0.0f));
		glm::mat4 sc = glm::scale(glm::vec3(agent.range, agent.range, agent.range));
		glm::mat4 modelview = tr * sc;
		basic->setUniform(uBModelMatrix, modelview);
		if (agent.alive)
		{
			glDrawElements(GL_LINE_STRIP, 17, GL_UNSIGNED_INT, (void*)0);
		}
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

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
	glDeleteBuffers(1, &avbo);
	glDeleteBuffers(1, &aibo);
	glfwTerminate();
}

void generateData()
{
	int sTime = (int)time(NULL);
	manager = new AgentManager(MAX_AGENTS);
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
	buildAgentMesh();
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