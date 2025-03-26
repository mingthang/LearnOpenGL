#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Model.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// sphere
const float PI = 3.14159265359f;
const unsigned int X_SEGMENTS = 8;
const unsigned int Y_SEGMENTS = 8;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Stencil Testing", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);

    // build and compile shaders
    // -------------------------
    Shader modelShader("model.vs", "model.fs");
    Shader planeShader("plane.vs", "plane.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float planeVertices[] = {
        // positions 
         1.0f,  1.0f,  0.1f, 
        -1.0f,  1.0f,  0.1f,  
        -1.0f, -1.0f,  0.1f,  

         1.0f,  1.0f,  0.1f,  
        -1.0f, -1.0f,  0.1f,  
         1.0f, -1.0f,  0.1f   
    };
    // plane VAO
    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    float cubeVertices[] = {
        // positions
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f
    };
    // cube VAO
    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    // Sphere
    std::vector<float> sphereVertices;
    for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
    {
        for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
        {
            float xSegment = (float)x / (float)X_SEGMENTS;
            float ySegment = (float)y / (float)Y_SEGMENTS;
            float xPos = 0.5f * cos(xSegment * 2.0f * PI) * sin(ySegment * PI);
            float yPos = 0.5f * cos(ySegment * PI);
            float zPos = 0.5f * sin(xSegment * 2.0f * PI) * sin(ySegment * PI);
            sphereVertices.push_back(xPos);
            sphereVertices.push_back(yPos);
            sphereVertices.push_back(zPos);
        }
    }
    std::vector<unsigned int> sphereIndices;
    for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
    {
        for (unsigned int x = 0; x < X_SEGMENTS; ++x)
        {
            unsigned int first = (y * (X_SEGMENTS + 1)) + x;
            unsigned int second = first + X_SEGMENTS + 1;

            sphereIndices.push_back(first);
            sphereIndices.push_back(second);
            sphereIndices.push_back(first + 1);

            sphereIndices.push_back(second);
            sphereIndices.push_back(second + 1);
            sphereIndices.push_back(first + 1);
        }
    }
    // Sphere VAO
    unsigned int sphereVAO, sphereVBO, sphereEBO;
    glGenVertexArrays(1, &sphereVAO);
    glGenBuffers(1, &sphereVBO);
    glGenBuffers(1, &sphereEBO);

    glBindVertexArray(sphereVAO);
    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(float), &sphereVertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(unsigned int), &sphereIndices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindVertexArray(0);

    // Pyramid
    float pyramidVertices[] = {
        -0.5f, 0.0f, -0.5f,  // V0
         0.5f, 0.0f, -0.5f,  // V1
         0.5f, 0.0f,  0.5f,  // V2

         -0.5f, 0.0f, -0.5f,  // V0
          0.5f, 0.0f,  0.5f,  // V2
         -0.5f, 0.0f,  0.5f,  // V3

         -0.5f, 0.0f, -0.5f,  // V0
          0.5f, 0.0f, -0.5f,  // V1
          0.0f, 1.0f,  0.0f,  // V4

          0.5f, 0.0f, -0.5f,  // V1
          0.5f, 0.0f,  0.5f,  // V2
          0.0f, 1.0f,  0.0f,  // V4

          0.5f, 0.0f,  0.5f,  // V2
         -0.5f, 0.0f,  0.5f,  // V3
          0.0f, 1.0f,  0.0f,  // V4

          -0.5f, 0.0f,  0.5f,  // V3
          -0.5f, 0.0f, -0.5f,  // V0
           0.0f, 1.0f,  0.0f   // V4
    };
    // Pyramid VAO
    unsigned int pyramidVAO, pyramidVBO;
    glGenVertexArrays(1, &pyramidVAO);
    glGenBuffers(1, &pyramidVBO);
    glBindVertexArray(pyramidVAO);
    glBindBuffer(GL_ARRAY_BUFFER, pyramidVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), &pyramidVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    // Diamond
    float diamondVertices[] = {
         0.0f,  0.5f,  0.0f,  // A
         0.5f,  0.0f,  0.0f,  // B
         0.0f,  0.0f,  0.5f,  // C

         0.0f,  0.5f,  0.0f,  // A
         0.0f,  0.0f,  0.5f,  // C
        -0.5f,  0.0f,  0.0f,  // D

         0.0f,  0.5f,  0.0f,  // A
        -0.5f,  0.0f,  0.0f,  // D
         0.0f,  0.0f, -0.5f,  // E

         0.0f,  0.5f,  0.0f,  // A
         0.0f,  0.0f, -0.5f,  // E
         0.5f,  0.0f,  0.0f,  // B

         0.0f, -0.5f,  0.0f,  // F
         0.0f,  0.0f,  0.5f,  // C
         0.5f,  0.0f,  0.0f,  // B

         0.0f, -0.5f,  0.0f,  // F
        -0.5f,  0.0f,  0.0f,  // D
         0.0f,  0.0f,  0.5f,  // C

         0.0f, -0.5f,  0.0f,  // F
         0.0f,  0.0f, -0.5f,  // E
        -0.5f,  0.0f,  0.0f,  // D

         0.0f, -0.5f,  0.0f,  // F
         0.5f,  0.0f,  0.0f,  // B
         0.0f,  0.0f, -0.5f   // E
    };
    // Diamond VAO
    unsigned int diamondVAO, diamondVBO;
    glGenVertexArrays(1, &diamondVAO);
    glGenBuffers(1, &diamondVBO);

    glBindVertexArray(diamondVAO);
    glBindBuffer(GL_ARRAY_BUFFER, diamondVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(diamondVertices), diamondVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindVertexArray(0);


    Model magicBox("./Models/MagicBox.fbx");

    // load textures
    // -------------

    // shader configuration
    // --------------------

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // Time logic, input, etc.
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);

        // Clear buffers
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // Camera
        glm::mat4 view = glm::mat4(1.0f);
        const float radius = 5.0f;
        float camX = sin(glfwGetTime() * 0.5f) * radius;
        float camZ = cos(glfwGetTime() * 0.5f) * radius;
        view = glm::lookAt(glm::vec3(camX, 0.0, camZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        // 1. Render the magic box model 
        modelShader.use();
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        modelShader.setMat4("projection", projection);
        modelShader.setMat4("view", view);
        modelShader.setMat4("model", model);
        modelShader.setVec3("modelColor", glm::vec3(1.0f, 1.0f, 1.0f));
        magicBox.Draw(modelShader);

        // 2. Create stencil masks for each window (first pass)
        glStencilMask(0xFF);                  // Enable writing to stencil
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); // Disable color writes

        // --- Window 1 Mask ---
        glStencilFunc(GL_ALWAYS, 1, 0xFF);      // Reference 1 for window 1
        glBindVertexArray(planeVAO);
        planeShader.use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.0f)); // Position mask for window 1
        //model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        planeShader.setMat4("model", model);
        planeShader.setMat4("view", view);
        planeShader.setMat4("projection", projection);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        // --- Window 2 Mask ---
        glStencilFunc(GL_ALWAYS, 2, 0xFF);      // Reference 2 for window 2
        glBindVertexArray(planeVAO);
		planeShader.use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.8f));
        planeShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        // --- Window 3 Mask ---
        glStencilFunc(GL_ALWAYS, 3, 0xFF);      // Reference 3 for window 3
        glBindVertexArray(planeVAO);
		planeShader.use();
        model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.0f));
        planeShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        // --- Window 4 Mask ---
        glStencilFunc(GL_ALWAYS, 4, 0xFF);      // Reference 4 for window 4
        glBindVertexArray(planeVAO);
		planeShader.use();
        model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.8f));
        planeShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        // Re-enable color and depth writes after updating stencil
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

        // 3. Render objects behind each window using the stencil test (second pass)
		// Disable writing to stencil
        glStencilMask(0x00);

        // --- Render object for Window 1 ---
        glDisable(GL_DEPTH_TEST);

        glStencilFunc(GL_EQUAL, 1, 0xFF);  // Only render where stencil == 1
        glBindVertexArray(cubeVAO);
        modelShader.use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        modelShader.setMat4("model", model);
        modelShader.setVec3("modelColor", glm::vec3(1.0, 0.5, 0.31));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        glEnable(GL_DEPTH_TEST);

        // --- Render object for Window 2 ---
        glDisable(GL_DEPTH_TEST);

        glStencilFunc(GL_EQUAL, 2, 0xFF);  // Only where stencil == 2
        glBindVertexArray(pyramidVAO);
        modelShader.use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f));
        modelShader.setMat4("model", model);
        modelShader.setVec3("modelColor", glm::vec3(1.0, 1.0, 0.0));
        glDrawArrays(GL_TRIANGLES, 0, 18);
        glBindVertexArray(0);

        glEnable(GL_DEPTH_TEST);

        // --- Render object for Window 3 ---
        glDisable(GL_DEPTH_TEST);

        glStencilFunc(GL_EQUAL, 3, 0xFF);  // Only where stencil == 3
        glBindVertexArray(sphereVAO);
        modelShader.use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.25f, 1.25f, 1.25f));
        modelShader.setMat4("model", model);
        modelShader.setVec3("modelColor", glm::vec3(1.0, 0.6, 0.8));
        glDrawElements(GL_TRIANGLES, sphereIndices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glEnable(GL_DEPTH_TEST);

        // --- Render object for Window 4 ---
        glDisable(GL_DEPTH_TEST);

        glStencilFunc(GL_EQUAL, 4, 0xFF);  // Only where stencil == 4
        glBindVertexArray(diamondVAO);
        modelShader.use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.25f, 1.25f, 1.25f));
        modelShader.setMat4("model", model);
        modelShader.setVec3("modelColor", glm::vec3(0.0f, 0.8f, 1.0f));
        glDrawArrays(GL_TRIANGLES, 0, 24);
        glBindVertexArray(0);

        glEnable(GL_DEPTH_TEST);

        // Reset stencil state for further rendering
        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 0, 0xFF);

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteBuffers(1, &planeVBO);
    glDeleteVertexArrays(1, &planeVAO);

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

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
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
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}