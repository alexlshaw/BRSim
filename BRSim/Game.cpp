#include "Game.h"

Game::Game(int levelWidth, int levelHeight)
{
	buildCircleMeshes();
	circleCentre = glm::vec2(levelWidth / 2.0f, levelHeight / 2.0f);
	circleRadius = glm::max<float>(levelWidth, levelHeight) * 1.5f;
	previousCircleRadius = circleRadius;
	previousCircleCentre = circleCentre;
	newCircle();
}

Game::~Game() {}

void Game::buildCircleMeshes()
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


//TODO: The game state tracker probably shouldn't also have responsibility for drawing the game state stuff
//But it's a pain in the neck to set up a separate scene manager to track all the mesh instances a la E01
void Game::drawCircles(Shader* basic, int uBModelMatrix)
{
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
}

void Game::update(float deltaTime)
{
	elapsedShrinkTime += deltaTime;
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

void Game::newCircle()
{
	nextCircleRadius = circleRadius / 2.0f;
	//compute random bearing and distance from current circle centre
	float angle = glm::radians((float)(rand() % 360));
	float distance = (float)(rand() % (int)nextCircleRadius);
	nextCircleCentre = circleCentre + glm::vec2(glm::cos(angle) * distance, glm::sin(angle) * distance);
	elapsedShrinkTime = 0.0f;
	//Originally manager had to cancel targets here
}

bool Game::isPositionInsideNextCircle(glm::vec2 position)
{
	float distanceToCentre = glm::length(position - nextCircleCentre);
	return distanceToCentre < nextCircleRadius;

}