#pragma once

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtx\transform.hpp" 
#include "glm\gtx\compatibility.hpp"
#include <memory>
#include <vector>

#include "AgentManager.h"
#include "Game.h"
#include "Level.h"
#include "Mesh.h"
#include "MeshTools.h"
#include "Shader.h"
#include "Texture.h"

//colours
const glm::vec4 white = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
const glm::vec4 black = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
const glm::vec4 red = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
const glm::vec4 translucentBlue = glm::vec4(0.0f, 0.0f, 1.0f, 0.35f);
const glm::vec4 green = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);

class Renderer
{
private:
	GLFWwindow* mainWindow;
	int screenWidth, screenHeight;
	//Variables for the basic graphics stuff
	std::unique_ptr<Shader> basic;
	std::unique_ptr<Shader> texturedUnlit;
	int uBProjMatrix, uBModelMatrix, uTProjMatrix, uTModelMatrix, uTex;

	Mesh circleOfDeathMesh, nextCircleMesh;
	Mesh agentMesh, agentTargetingCircleMesh, agentHealthBackMesh, agentHealthFrontMesh;
	Mesh lineMesh;
	Mesh levelMesh;
	Mesh itemMesh;
	Texture itemTex;
	std::vector<Vertex> linePoints;
	
	bool showTargetingLines;
	bool showLevelWalkData;
	bool showHealthBars;
	bool showRangeAndVision;

	void initOpenGL();
	void loadShaders();
	void buildAgentMeshes();
	void buildCircleMeshes();
	void buildLevelMesh(const Level& level);
	void buildItemMesh();
	void drawLine(glm::vec2 start, glm::vec2 end, glm::vec4 colour); //this function doesn't actually draw a line, it just adds the line data to the batch to be drawn later
	void drawLines();//this function takes the accumulated line data, and sends it off to the GPU to be drawn
	void drawAgents(const AgentManager& manager);
	void drawCircles(const Game& gameState);
	void drawLevel(const Level& level);
	void drawItems(const Game& gameState);
	glm::mat4 computeProjection();
public:
	glm::vec2 windowOffset;
	glm::vec2 windowOffsetAtPanStart;
	float zoomLevel;
	
	Renderer(GLFWwindow* mainWindow, const Level& level);
	~Renderer();
	void draw(const Game& gameState, const Level& level, const AgentManager& manager);
	void toggleShowTargetingLines();
	void toggleShowLevelWalkData();
	void toggleShowHealthBars();
	void toggleShowRangeAndVision();
};