// Per-vertex Shading (Gouraud Shading)
#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 resultColor;

struct Material
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct Light
{
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
	// ambient
	vec3 ambient = light.ambient * material.ambient;

	// diffuse
	vec3 norm = normalize(mat3(transpose(inverse(model))) * aNormal);
	vec3 worldPos = vec3(model * vec4(aPos, 1.0)); // here we do the lighting calculations in world space.
	vec3 lightDir = normalize(light.position - worldPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * (diff * material.diffuse);

	// specular
	vec3 viewDir = normalize(viewPos - worldPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * (spec * material.specular);

	// result color
	//resultColor = ambient + diffuse + specular;
	resultColor = clamp(ambient + diffuse + specular, 0.0, 1.0);
	
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}
