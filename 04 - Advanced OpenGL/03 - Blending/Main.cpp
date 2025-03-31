#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "stb_image.h"
#include "Camera.h"

#define NEAR_PLANE 0.1f
#define FAR_PLANE 100.0f

// Window Settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Camera 
Camera camera(glm::vec3(0.0f, 0.0f, 3.5f));
bool firstMouse = true;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// FIRE
bool leftMousePressed = false;

// Debug
bool isDebugMode = false;

void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void MouseCallback(GLFWwindow* window, double xposIn, double yposIn);
void processInput(GLFWwindow* window);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void ToggleDebugMode();

// Ray casting Prototypes
void Fire();
void RayCast();
glm::vec3 GetRayDir(double mouseX, double mouseY, glm::mat4 projection, glm::mat4 view);

//
glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, NEAR_PLANE, FAR_PLANE);

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

	while (!glfwWindowShouldClose(window)) {
		// per-frame time logic
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glClearColor(0.1, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// DEBUG


		// Process input
		processInput(window);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

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
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		Fire(window);
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

void Fire(GLFWwindow* window) {
	RayCast(window);
}

void RayCast(GLFWwindow* window) {
	glm::vec3 rayOrigin = camera.Position;
	glm::vec3 rayDir = GetRayDir(window, camera.GetViewMatrix());
}

glm::vec3 GetRayDir(GLFWwindow* window, glm::mat4 view) {
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