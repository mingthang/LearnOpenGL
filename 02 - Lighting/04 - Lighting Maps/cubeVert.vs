// Vertex Shader (Per-Vertex Lighting, Gouraud Shading)
#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragColor;
out vec2 TexCoords;

struct Material {
    sampler2D diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main()
{
    TexCoords = aTexCoords; // Pass texture coordinates to fragment shader

    // Ambient
    vec3 ambient = light.ambient;

    // Normal transformation
    vec3 norm = normalize(mat3(transpose(inverse(model))) * aNormal);
    vec3 worldPos = vec3(model * vec4(aPos, 1.0));
    vec3 lightDir = normalize(light.position - worldPos);

    // Diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff;

    // Specular
    vec3 viewDir = normalize(viewPos - worldPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);

    // Final color (without texture, texture will be applied in fragment shader)
    FragColor = ambient + diffuse + specular;

    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
