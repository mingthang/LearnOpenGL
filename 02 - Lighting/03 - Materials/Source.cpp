#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"

#include <iostream>

void processInput(GLFWwindow* window);
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);

// Settings
const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 768;

// Camera 
Camera camera(glm::vec3(2.0f, 0.0f, 0.0f));
bool firstMouse = true;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool mouseCallbackEnabled = true;
bool vKeyPressed = false;

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Lighting
glm::vec3 lightOffset(1.2f, 1.0f, 2.0f);
glm::vec3 randomAxis = glm::normalize(glm::vec3(
	(rand() % 200 - 100) / 100.0f,  // Random X (-1 to 1)
	(rand() % 200 - 100) / 100.0f,  // Random Y (-1 to 1)
	(rand() % 200 - 100) / 100.0f   // Random Z (-1 to 1)
));
float rotationAngle = 0.0f;
float rotationSpeed = 1.0f;

// ImGUI
const char* shadingMethods[] = { "Gouraud", "Phong" };
static int shadingMethods_current = 0;
// Material properties
glm::vec3 materialAmbient(1.0f, 0.5f, 0.31f);
glm::vec3 materialDiffuse(1.0f, 0.5f, 0.31f);
glm::vec3 materialSpecular(1.0f, 0.5f, 0.31f);
float materialShininess = 32.0f;
// Light properties
glm::vec3 lightColor = glm::vec3(1.0f);
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
glm::vec3 lightAmbient(0.2f, 0.2f, 0.2f);
glm::vec3 lightDiffuse(0.7f, 0.7f, 0.7f);
glm::vec3 lightSpecular(1.0f, 1.0f, 1.0f);

int main()
{
	// Initialize GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL - Materials", NULL, NULL);
	if (!window)
	{
		std::cout << "Failed to create glfw window." << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
	glfwSetCursorPosCallback(window, MouseCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetWindowPos(window, 450, 180);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// Shaders
	Shader phongShader = Shader("cubeFrag.vs", "cubeFrag.fs");
	Shader gouraudShader = Shader("cubeVert.vs", "cubeVert.fs");
	Shader lightCubeShader = Shader("lightCube.vs", "lightCube.fs");

	Shader* cubeShader = &gouraudShader;

	// Vertex Data
	float vertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};

	// Configure the cube's VAO (and VBO)
	unsigned int VBO, cubeVAO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(cubeVAO);
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1);

	// Configure the light's VAO (VBO stays the same, also a 3D cube)
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);

	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)0);
	glEnableVertexAttribArray(0);

	// Configure global OpenGL state
	glEnable(GL_DEPTH_TEST);
	camera.LookAt(glm::vec3(0.0f));

	// ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 460");

	// Render loop
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Process input
		processInput(window);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Draw light cube
		lightCubeShader.use();
		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		lightCubeShader.setMat4("projection", projection);
		lightCubeShader.setMat4("view", view);
		// rotation
		rotationAngle += deltaTime * rotationSpeed;
		if (rotationAngle >= glm::two_pi<float>())
		{
			rotationAngle = glm::two_pi<float>() - rotationAngle;
			randomAxis = glm::normalize(glm::vec3(
				(rand() % 200 - 100) / 100.0f,
				(rand() % 200 - 100) / 100.0f,
				(rand() % 200 - 100) / 100.0f
			));
		}
		glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), rotationAngle, randomAxis);
		glm::vec3 lightPos = glm::vec3(rotation * glm::vec4(lightOffset, 1.0f));
		// model transformation
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f));
		lightCubeShader.setMat4("model", model);
		// render the light cube
		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// Draw coral cube
		cubeShader->use();
		// set uniforms
		cubeShader->setVec3("material.ambient", materialAmbient);
		cubeShader->setVec3("material.diffuse", materialDiffuse);
		cubeShader->setVec3("material.specular", materialSpecular);
		cubeShader->setFloat("material.shininess", materialShininess);
		
		cubeShader->setVec3("light.ambient", lightColor * lightAmbient);
		cubeShader->setVec3("light.diffuse", lightColor * lightDiffuse);
		cubeShader->setVec3("light.specular", lightSpecular);
		cubeShader->setVec3("light.position", lightPos);
		cubeShader->setVec3("viewPos", camera.Position);
		// view/projection transformations
		cubeShader->setMat4("projection", projection);
		cubeShader->setMat4("view", view);
		// model transformation
		model = glm::mat4(1.0f);
		cubeShader->setMat4("model", model);
		// render the cube
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		ImGui::Begin("Options");
		if(ImGui::Combo("Shading Methods", &shadingMethods_current, shadingMethods, IM_ARRAYSIZE(shadingMethods)))
			switch (shadingMethods_current) {
				case 0: cubeShader = &gouraudShader; break;
				case 1: cubeShader = &phongShader; break;
			}	
		ImGui::End();

		ImGui::Begin("Material and Light Settings");

		// Material Sliders
		ImGui::Text("Material Properties");
		ImGui::ColorEdit3("Ambient", (float*)&materialAmbient);
		ImGui::ColorEdit3("Diffuse", (float*)&materialDiffuse);
		ImGui::ColorEdit3("Specular", (float*)&materialSpecular);
		ImGui::SliderFloat("Shininess", &materialShininess, 1.0f, 128.0f);

		// Light Sliders
		ImGui::Separator();
		ImGui::Text("Light Properties");
		ImGui::ColorEdit3("Light Color", (float*)&lightColor);
		ImGui::ColorEdit3("Light Ambient", (float*)&lightAmbient);
		ImGui::ColorEdit3("Light Diffuse", (float*)&lightDiffuse);
		ImGui::ColorEdit3("Light Specular", (float*)&lightSpecular);

		ImGui::End();
		
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();

	return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE))
		glfwSetWindowShouldClose(window, true);

	// toggle camera
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS && !vKeyPressed)
	{
		if (mouseCallbackEnabled)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		else
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		mouseCallbackEnabled = !mouseCallbackEnabled;
		vKeyPressed = true;  
	}
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_RELEASE)
	{
		vKeyPressed = false;  
	}

	if (glfwGetKey(window, GLFW_KEY_1))
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if (glfwGetKey(window, GLFW_KEY_2))
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

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

void MouseCallback(GLFWwindow* window, double xPosIn, double yPosIn)
{
	if (!mouseCallbackEnabled)
		return;

	float xPos = static_cast<float>(xPosIn);
	float yPos = static_cast<float>(yPosIn);

	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	float xOffset = xPos - lastX;
	float yOffset = lastY - yPos;

	lastX = xPos;
	lastY = yPos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}
