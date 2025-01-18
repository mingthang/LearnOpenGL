#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/noise.hpp>

#include "Shader.h"
#include "Camera.h"

#include <iostream>
#include <vector>

// Window 
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Terrain
const int cols = 100;
const int rows = 100;
const float scale = 0.2f;

// Camera 
Camera camera(glm::vec3(0.0f, 1.0f, 3.0f));
bool firstMouse = true;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

// Timing
float deltaTime = 0.0f;	
float lastFrame = 0.0f;

// Function prototypes
void processInput(GLFWwindow* window);
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
std::vector<float> generatePlaneVertexData(float width, float height, int xDivisions, int yDivisions);
std::vector<unsigned int> generatePlaneIndexData(int xDivisions, int yDivisions);
void updateVertexHeight(std::vector<float>& vertices, int xDivisions, int yDivisions);

int main()
{
	// Initialize GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL - 07 Camera", NULL, NULL);
	if (!window)
	{
		std::cout << "Failed to create GLFW window." << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetScrollCallback(window, scroll_callback);

	// Load OpenGL functions using GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD." << std::endl;
		return -1;
	}

	// Shader program
	Shader shaderProgram("4.6.vertex_shader.glsl", "4.6.fragment_shader.glsl");

	// Generate terrain data
	std::vector<float> vertices = generatePlaneVertexData(3, 2, 30, 20);

	// Generate and bind VAO/VBO
	unsigned int VAO, VBO, EBO;

	// VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// VBO
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

	// Vertex attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
	glEnableVertexAttribArray(0);

	// EBO
	std::vector<unsigned int> indices = generatePlaneIndexData(30, 20);
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);

	// Configure global OpenGL state
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Render loop
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Process input
		processInput(window);

		// Update terrain heights 
		updateVertexHeight(vertices, 30, 20);
		// Re-upload vertex data to the GPU
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());

		// Clear color buffer & depth buffer
		glClearColor(1.0, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Render terrain
		shaderProgram.use();

		// world transformation
		glm::mat4 model = glm::mat4(1.0f);
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		shaderProgram.setMat4("model", model);

		// view transformation
		glm::mat4 view = glm::mat4(1.0f);
		view = camera.GetViewMatrix();
		shaderProgram.setMat4("view", view);

		// projection
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		shaderProgram.setMat4("projection", projection);

		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// GLFW: swap buffers and poll IO events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Cleanup
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glfwTerminate();

	return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE))
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

std::vector<float> generatePlaneVertexData(float width, float height, int xDivisions, int yDivisions)
{
	std::vector<float> vertices;

	// Calculate center and step factors
	float x2 = width / 2.0f;
	float y2 = height / 2.0f;
	float iFactor = height / static_cast<float>(yDivisions);
	float jFactor = width / static_cast<float> (xDivisions);

	// Pre-allocate space for the vertices
	int numVertices = (xDivisions + 1) * (yDivisions + 1);
	vertices.reserve(numVertices * 3);

	for (int i = 0; i <= yDivisions; ++i)
	{
		// y-coordinate
		float y = (iFactor * i - y2);
		for (int j = 0; j <= xDivisions; ++j)
		{
			// x-coordinate
			float x = (jFactor * j - x2);

			vertices.push_back(x);
			vertices.push_back(height);
			vertices.push_back(y);
		}
	}

	return vertices;
}

std::vector<unsigned int> generatePlaneIndexData(int xDivisions, int yDivisions)
{
	std::vector<unsigned int> indices;

	for (int i = 0; i < yDivisions; ++i)
	{
		unsigned int rowStart = i * (xDivisions + 1);
		unsigned int nextRowStart = (i + 1) * (xDivisions + 1);

		for (int j = 0; j < xDivisions; ++j)
		{
			int bottomLeft = rowStart + j;
			int bottomRight = bottomLeft + 1;
			int topLeft = nextRowStart + j;
			int topRight = topLeft + 1;

			// Two triangles per square, one for each side
			indices.push_back(topLeft);
			indices.push_back(bottomLeft);
			indices.push_back(topRight);

			indices.push_back(bottomLeft);
			indices.push_back(bottomRight);
			indices.push_back(topRight);
		}
	}

	return indices;
}

void updateVertexHeight(std::vector<float>& vertices, int xDivisions, int yDivisions)
{
	// Ensure we have valid vertices
	if (vertices.empty()) return;

	// Perlin noise parameters
	float noiseScale = 0.5f;  // Smaller = smoother terrain
	float time = static_cast<float>(glfwGetTime());

	int index = 0; // Tracks vertex index
	for (int i = 0; i <= yDivisions; ++i)
	{
		for (int j = 0; j <= xDivisions; ++j)
		{
			float x = vertices[index];     
			float z = vertices[index + 2]; 

			// Calculate the new y value using Perlin noise
			float height = glm::perlin(glm::vec2(x, z) * noiseScale + time * 0.5f) * 0.5f;

			// Update the y-coordinate
			vertices[index + 1] = height; 

			index += 3; // Move to the next vertex (x, y, z)
		}
	}
}
