#include "Renderer.h"

Renderer::Renderer(GLFWwindow* mainWindow)
{
	this->mainWindow = mainWindow;
	showLevelWalkData = false;
	showTargetingLines = false;
	zoomLevel = 1.0f;
	windowOffset = glm::vec2(0.0f, 0.0f);
	windowOffsetAtPanStart = windowOffset;
	initOpenGL();
	buildCircleMeshes();
	buildAgentMeshes();
}

Renderer::~Renderer() 
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
}

void Renderer::initOpenGL()
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

	//Extract the size of the window we're working with
	glfwGetFramebufferSize(mainWindow, &screenWidth, &screenHeight);
	glViewport(0, 0, screenWidth, screenHeight);
}

void Renderer::buildCircleMeshes()
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	int sides = 32;
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

void Renderer::buildAgentMeshes()
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
}

void Renderer::draw(Game* gameState, Level* level, AgentManager* manager)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	linePoints.clear();
	glm::mat4 projection = computeProjection();

	//first draw the level
	texturedUnlit->use();
	texturedUnlit->setUniform(uTProjMatrix, projection);
	texturedUnlit->setUniform(uTex, 0);
	level->draw(texturedUnlit, uTModelMatrix, showLevelWalkData);

	//draw the agents
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

	//draw circles
	drawCircles(gameState);

	//draw level boundary
	drawLine(glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, level->height), black);
	drawLine(glm::vec2(0.0f, 0.0f), glm::vec2(level->width, 0.0f), black);
	drawLine(glm::vec2(level->width, level->height), glm::vec2(0.0f, level->height), black);
	drawLine(glm::vec2(level->width, level->height), glm::vec2(level->width, 0.0f), black);

	drawLines();


	glfwSwapBuffers(mainWindow);
}

void Renderer::drawLine(glm::vec2 start, glm::vec2 end, glm::vec4 colour)
{
	linePoints.push_back(Vertex(glm::vec4(start.x, start.y, 0.0f, 1.0f), colour));
	linePoints.push_back(Vertex(glm::vec4(end.x, end.y, 0.0f, 1.0f), colour));
}

void Renderer::drawLines()
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

void Renderer::drawCircles(Game* gameState)
{
	//draw circle of death
	glm::mat4 tr = glm::translate(glm::vec3(gameState->circleCentre.x, gameState->circleCentre.y, 0.0f));
	glm::mat4 sc = glm::scale(glm::vec3(gameState->circleRadius, gameState->circleRadius, gameState->circleRadius));
	glm::mat4 modelview = tr * sc;
	basic->setUniform(uBModelMatrix, modelview);
	circleOfDeathMesh.draw();

	//draw upcoming circle
	tr = glm::translate(glm::vec3(gameState->nextCircleCentre.x, gameState->nextCircleCentre.y, 0.0f));
	sc = glm::scale(glm::vec3(gameState->nextCircleRadius, gameState->nextCircleRadius, gameState->nextCircleRadius));
	modelview = tr * sc;
	basic->setUniform(uBModelMatrix, modelview);
	nextCircleMesh.draw(GL_LINE_LOOP);
}

glm::mat4 Renderer::computeProjection()
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

void Renderer::toggleShowTargetingLines()
{
	showTargetingLines = !showTargetingLines;
}

void Renderer::toggleShowLevelWalkData()
{
	showLevelWalkData = !showLevelWalkData;
}