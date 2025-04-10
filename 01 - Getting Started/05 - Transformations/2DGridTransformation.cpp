#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include "Shader.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
std::vector<float> generateGridVertices();

// Settings for Window size
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Constant
float EPSILON = 1e-6f;

// Settings for the grid
const float gridSpacing = 0.2; 
const int gridLines = 20;       

int main()
{

	// GLFW: Initialization and Configuration
	// ======================================
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// GLFW: Window Creation
	// =====================
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL - Transformation", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window." << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// GLAD: Initialization and Load all OpenGL function pointers
	// ==========================================================
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// Shader
	// =======
	Shader shaderProgram("4.6.vertex_shader.glsl", "4.6.fragment_shader.glsl");

	// Set up vertex data (and buffer(s)) and configure vertex attributes
	// ==================================================================
	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	std::vector<float> gridVertices = generateGridVertices();
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, gridVertices.size() * sizeof(float), gridVertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Render Loop
	// -----------
	// Color Transition Time
	float time = 0.0f;
	float rotationAngle = 0.0f;
	float z_r2 = 0.0f;
	bool isShear = false;
	bool isRotate = false;
	bool isInverse = false;
	bool is1D = false;
	bool reverse = false;

	while (!glfwWindowShouldClose(window))
	{
		// Input
		// =====
		processInput(window);

		// Render
		// ======
		//glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// create transformations
		glm::mat4 transform = glm::mat4(1.0f); 
		// Shear
		if (!isShear)
		{
			if (reverse)
			{
				time = 1.5f - glfwGetTime();
				if (time <= 0.0f) 
				{
					isShear = true;
					reverse = false; 
					glfwSetTime(0.0f);
					time = glfwGetTime();
				}
			}
			else
			{
				time = glfwGetTime();
				if (time >= 1.5f)
				{
					reverse = true; 
					glfwSetTime(0.0f);
					time = glfwGetTime();
				}
			}
			transform[1][0] = time * 0.1f;
		}
		// Rotate
		if (isShear && !isRotate) 
		{
			if (reverse)
			{
				time = 2.0f - glfwGetTime();
				if (time <= 0.0f) 
				{
					isRotate = true;
					reverse = false; 
					glfwSetTime(0.0f);
					time = glfwGetTime();
				}
			}
			else
			{
				time = glfwGetTime();
				if (time >= 2.0f)
				{
					reverse = true;
					glfwSetTime(0.0f);
					time = glfwGetTime();
				}
			}
			rotationAngle = time * 15.f;
			transform = glm::rotate(transform, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			if (isRotate)
				time = 1.0f;
		}
		// inverse
		if (isRotate && !isInverse)
		{
			time -= glfwGetTime() * 0.0001f;
			z_r2 += glfwGetTime() * 0.0001f;
			if (time <= -1.0f)
			{
				glfwSetTime(0.0f);
				isInverse = true;
				time = 1.0f;
			}
			transform = glm::mat4();
			transform[0] = glm::vec4(time, 0.0, 0.0, 0.0);
			transform[1] = glm::vec4(0.0, time, 0.0, 0.0);
			transform[2] = glm::vec4(0.0, 0.0, 1.0, 0.0);
			transform[3] = glm::vec4(0.0, 0.0, 0.0, 1.0);
			if (isInverse)
				time = 0.0f;
		}
		// 1D
		if (isInverse && !is1D)
		{
			if (reverse) 
			{
				time -= glfwGetTime() * 0.0001f;
				if (time <= 0.0f)
				{
					glfwSetTime(0.0f);
					time = 0.0f;
					is1D = true;
					reverse = false;
				}
				transform = glm::mat4();
				transform[0] = glm::vec4(1.0, 0.0, 0.0, 0.0);
				transform[1] = glm::vec4(time, 1.0 - time, 0.0, 0.0);
				transform[2] = glm::vec4(0.0, 0.0, 1.0, 0.0);
				transform[3] = glm::vec4(0.0, 0.0, 0.0, 1.0);
			}
			else
			{
				time += glfwGetTime() * 0.0001f;
				if (time >= 1.0f)
				{
					glfwSetTime(0.0f);
					reverse = true;
				}
				transform = glm::mat4();
				transform[0] = glm::vec4(1.0, 0.0, 0.0, 0.0);
				transform[1] = glm::vec4(time, 1.0 - time, 0.0, 0.0);
				transform[2] = glm::vec4(0.0, 0.0, 1.0, 0.0);
				transform[3] = glm::vec4(0.0, 0.0, 0.0, 1.0);
			}
		}

		// Shader Program
		shaderProgram.use();
		shaderProgram.setMat4("transform", transform);

		glBindVertexArray(VAO);
		glDrawArrays(GL_LINES, 0, gridVertices.size() / 3.f);
		glBindVertexArray(0);

		// GLFW: Swap buffers and Poll I/O events
		// ======================================
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Deallocate all resources
	// ========================
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	// GLFW: Terminate, clearing all previously allocated GLFW resources.
	// ==================================================================
	glfwTerminate();

	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

// GLFW: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window resolution
	glViewport(0, 0, width, height);
}

std::vector<float> generateGridVertices() {
	std::vector<float> vertices;

	// vertical lines
	for (int i = -gridLines; i <= gridLines; ++i) {
		float x = i * gridSpacing;
		vertices.push_back(x);
		vertices.push_back(-gridLines * gridSpacing);
		vertices.push_back(0.0f);

		vertices.push_back(x);
		vertices.push_back(gridLines * gridSpacing);
		vertices.push_back(0.0f);
	}

	// horizontal lines
	for (int i = -gridLines; i <= gridLines; ++i) {
		float y = i * gridSpacing;
		vertices.push_back(-gridLines * gridSpacing);
		vertices.push_back(y);
		vertices.push_back(0.0f);

		vertices.push_back(gridLines * gridSpacing);
		vertices.push_back(y);
		vertices.push_back(0.0f);
	}

	return vertices;
}
