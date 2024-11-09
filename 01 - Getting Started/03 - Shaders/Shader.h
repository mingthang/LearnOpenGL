#pragma once

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
public:
    Shader(const char* vertexShaderPath, const char* fragmentShaderPath);

    // active shader
    void use();

    // utility uniform functions
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec3(const std::string& name, glm::vec3 value) const;
    void setVec3Array(const std::string& name, glm::vec3 value[], size_t count) const;

    // Shader Program ID
    unsigned int ID;
private:
    void checkCompileErrors(unsigned int shader, std::string type);
};
