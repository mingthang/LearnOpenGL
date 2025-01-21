#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

#include <iostream>
#include <stack>

unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;

// Camera
float cameraX = 0.0f;
float cameraY = 0.0f;
float cameraZ = 12.0f;

// Utils
GLuint VAO, VBO[2];
glm::mat4 pMat, vMat, mMat, mvMat;
Shader* shaderProgram;

std::stack<glm::mat4> matrixStack;

// Function prototypes
void processInput(GLFWwindow* window);
void framebuffersize_callback(GLFWwindow* window, int width, int height);
void setupVertices();
void display();

int main()
{
	// Initialize GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Matrix Stack", NULL, NULL);
	if (!window)
	{
		std::cout << "Failed to create GLFW window." << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffersize_callback);

	// Load OpenGL functions using GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD." << std::endl;
		return -1;
	}

	// Shader
	shaderProgram = new Shader("4.6.vertShader.glsl", "4.6.fragShader.glsl");

	setupVertices();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	while (!glfwWindowShouldClose(window))
	{
		// Process Input
		processInput(window);

		display();

		// GLFW: swap buffers and poll IO events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE))
		glfwSetWindowShouldClose(window, true);
}

void framebuffersize_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void setupVertices()
{
	float cube[108] = {
		-1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f,
	};

	float pyramid[54] = {
		-1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		 1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f
	};

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(2, VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);	
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramid), pyramid, GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void display()
{
	
	float currentTime = (float)glfwGetTime();

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shaderProgram->use();

	// Projection matrix
	pMat = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
	shaderProgram->setMat4("projection", pMat);

	// View matrix
	vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));
	matrixStack.push(vMat);

	// sun
	matrixStack.push(matrixStack.top());
	matrixStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f));
	matrixStack.push(matrixStack.top());
	matrixStack.top() *= glm::rotate(glm::mat4(1.0f), currentTime, glm::vec3(1.0f, 0.0f, 0.0f));
	shaderProgram->setMat4("model", matrixStack.top());

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
	glEnableVertexAttribArray(0);
	glDrawArrays(GL_TRIANGLES, 0, 18);

	matrixStack.pop();

	// planet
	matrixStack.push(matrixStack.top());
	matrixStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(sin(currentTime) * 4.0f, 0.0f, cos(currentTime) * 4.0f));
	matrixStack.push(matrixStack.top());
	matrixStack.top() *= glm::rotate(glm::mat4(1.0f), currentTime, glm::vec3(0.0f, 1.0f, 0.0f));
	shaderProgram->setMat4("model", matrixStack.top());

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
	glEnableVertexAttribArray(0);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	matrixStack.pop();

	// moon
	matrixStack.push(matrixStack.top());
	matrixStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, sin(currentTime) * 2.0f, cos(currentTime) * 2.0f));
	matrixStack.push(matrixStack.top());
	matrixStack.top() *= glm::rotate(glm::mat4(1.0f), currentTime, glm::vec3(0.0f, 0.0f, 1.0f));
	matrixStack.top() *= glm::scale(glm::mat4(1.0f), glm::vec3(0.25f, 0.25f, 0.25f));
	shaderProgram->setMat4("model", matrixStack.top());

	glDrawArrays(GL_TRIANGLES, 0, 36);
	
	matrixStack.pop();

	while (!matrixStack.empty())
		matrixStack.pop();
}
