#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <chrono>
#include <thread>
#include "Shader.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
float EPSILON = 1e-6f; 

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
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL - Shader Triangle", NULL, NULL);
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
	float vertices[] = {
		// position				// color
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f, 0.0f,	// right RED
		-0.5f, -0.5f, 0.0f,		0.0f, 1.0f, 0.0f,	// left GREEN
		0.0f, 0.5f, 0.0f,		0.0f, 0.0f, 1.0f	// top BLUE
	};
	/*
	float vertices[] = {
		// position		
		0.5f, -0.5f, 0.0f,	// right
		-0.5f, -0.5f, 0.0f,	// left
		0.0f, 0.5f, 0.0f			// top
	};
	*/

	// VAO, VBO
	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	// color
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

	// Render Loop
	// -----------
	// Color Transition Time
	float time;

	while (!glfwWindowShouldClose(window))
	{
		// Input
		// =====
		processInput(window);

		// Render
		// ======
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Shader Program
		shaderProgram.use();
		time = glfwGetTime() * 0.25f;
		shaderProgram.setFloat("time", time);

		// Draw Triangle
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

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