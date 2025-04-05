#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "stb_image.h"
#include "Camera.h"
#include "Shader.h"
#include <vector>

#define NEAR_PLANE 0.1f
#define FAR_PLANE 100.0f
#define EPSILON 1e-6f

// Window Settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Camera 
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool firstMouse = true;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// FIRE
bool leftMouseDown = false;
bool leftMouseDownLastFrame = false;

// Debug
bool isDebugMode = false;

void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void MouseCallback(GLFWwindow* window, double xposIn, double yposIn);
void processInput(GLFWwindow* window);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void ToggleDebugMode();

// Prototypes
unsigned int loadTexture(const char* path);
void Fire(GLFWwindow* window, glm::mat4 projection, glm::mat4 view);
glm::vec3 RayCast(GLFWwindow* window, glm::mat4 projection, glm::mat4 view);
//glm::vec3 GetRayDir(double mouseX, double mouseY, glm::mat4 projection, glm::mat4 view);
glm::vec3 GetRayDir(GLFWwindow* window, glm::mat4 projection, glm::mat4 view);

// Transformations
glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH/(float)SCR_HEIGHT, NEAR_PLANE, FAR_PLANE);

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;
};

int main() {

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "03 - Blending", nullptr, nullptr);
	if (!window) {
		std::cout << "Failed to create GLFW window." << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
	glfwSetCursorPosCallback(window, MouseCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetMouseButtonCallback(window, MouseButtonCallback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD to load OpenGL function pointers" << std::endl;
		return -1;
	}

	glEnable(GL_DEPTH_TEST);

	std::vector<Vertex> cubeVertices[] = {
		// positions			 // normals			  // uv
		 {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
		 {{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
		 {{0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}
	};
	float planeVertices[] = {
		// positions			// normals			// texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
		 5.0f, -0.5f,  5.0f,	0.0f, 1.0f, 0.0f,	2.0f, 0.0f,
		-5.0f, -0.5f,  5.0f,	0.0f, 1.0f, 0.0f,	0.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,	0.0f, 1.0f, 0.0f,	0.0f, 2.0f,

		 5.0f, -0.5f,  5.0f,	0.0f, 1.0f, 0.0f,	2.0f, 0.0f,
		-5.0f, -0.5f, -5.0f, 	0.0f, 1.0f, 0.0f,	0.0f, 2.0f,
		 5.0f, -0.5f, -5.0f,	0.0f, 1.0f, 0.0f,	2.0f, 2.0f
	};
	// Cube VAO
	unsigned int cubeVAO, cubeVBO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glBindVertexArray(0);
	// Plane VAO
	unsigned int planeVAO, planeVBO;
	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);
	glBindVertexArray(planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glBindVertexArray(0);

	// Textures
	GLuint cubeTexID = loadTexture("./marble.jpg");
	GLuint planeTexID = loadTexture("./metal.png");

	// Shaders & Configuration
	Shader envShader("envShader.vs", "envShader.fs");

	while (!glfwWindowShouldClose(window)) {
		// per-frame time logic
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glClearColor(0.1, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Transformations
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = camera.GetViewMatrix();

		// Environment Rendering
		envShader.use();
		envShader.setMat4("view", view);
		envShader.setMat4("projection", projection);
		// Render cubes
		glBindVertexArray(cubeVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, cubeTexID);
		model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
		envShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
		envShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		// Render floor
		glBindVertexArray(planeVAO);
		//glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, planeTexID);
		model = glm::mat4(1.0f);
		envShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		// Debug
		if (isDebugMode) {

		}

		if (leftMouseDown) {
			std::cout << "PEW" << std::endl;
			leftMouseDown = false;
		}

		// Process input
		processInput(window);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteBuffers(1, &cubeVBO);
	glDeleteVertexArrays(1, &planeVAO);
	glDeleteBuffers(1, &planeVBO);
	glDeleteTextures(1, &cubeTexID);
	glDeleteTextures(1, &planeTexID);

	glfwTerminate();

	return 0;
}

void ToggleDebugMode() {
	isDebugMode = (isDebugMode == true) ? false : true;
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE))
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_GRAVE_ACCENT))
		ToggleDebugMode();

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (action == GLFW_PRESS && !leftMouseDownLastFrame)
			leftMouseDown = true;
		else 
			leftMouseDown = false;
		leftMouseDownLastFrame = leftMouseDown;
	}
}

void MouseCallback(GLFWwindow* window, double xposIn, double yposIn) {
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

unsigned int loadTexture(const char* path) {
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data) {
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		stbi_image_free(data);
	}
	else {
		std::cout << "Texture failed to load at path " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

void Fire(GLFWwindow* window) {
	glm::vec3 HitPoint = RayCast(window, projection, camera.GetViewMatrix());
}

glm::vec3 RayCast(GLFWwindow* window, glm::mat4 projection, glm::mat4 view) {
	glm::vec3 rayOrigin = camera.Position;
	glm::vec3 rayDir = GetRayDir(window, projection, view);

	//glm::vec3 HitPoint = RayTriangl
	return glm::vec3(0);
}

glm::vec3 GetRayDir(GLFWwindow* window, glm::mat4 projection, glm::mat4 view) {
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	float rasterX = (float)width / 2.0f;
	float rasterY = (float)height / 2.0f;

	// Raster space -> NDC transform
	float x = (2.0f * rasterX) / (float)width - 1.0f;
	float y = -((2.0f * rasterY) / (float)height - 1.0f);
	float z = 1.0f;
	glm::vec4 rayClip(x, y, -1.0f, 1.0f);
	glm::vec4 rayEye = glm::inverse(projection) * rayClip;
	rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

	glm::vec3 rayWorldDir = glm::vec3(glm::inverse(view) * rayEye);
	rayWorldDir = glm::normalize(rayWorldDir);

	return rayWorldDir;
}

bool RayPlaneIntersection(const glm::vec3 &rayOrigin, const glm::vec3 &rayDir, const glm::vec3 &planePoint, const glm::vec3 &planeNormal, glm::vec3 &hitPoint) {
	// P(t) = rayOrigin + t*rayDir
	// N \cdot (P - P(t)) = 0 <=> N \cdot (P - rayOrigin + t*rayDir) = 0
	// => t = [N \cdot (P - rayOrigin)] / [N \cdot rayDir]

	float t = 0;
	float denom = glm::dot(planeNormal, rayDir);
	if (fabs(denom) < EPSILON)
		return false;

	t = glm::dot(planeNormal, (planePoint - rayOrigin)) / denom;
	if (t >= 0.0f) {
		hitPoint = rayOrigin + t * rayDir;
		return true;
	}
	return false;
}