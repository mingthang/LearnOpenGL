// Per-vertex Shading (Gouraud Shading)
#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 resultColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
	// ambient
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * lightColor;

	// diffuse
	vec3 norm = normalize(mat3(transpose(inverse(model))) * aNormal);
	vec3 worldPos = vec3(model * vec4(aPos, 1.0)); // here we do the lighting calculations in world space.
	vec3 lightDir = normalize(lightPos - worldPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	// specular
	vec3 viewDir = normalize(viewPos - worldPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float specularStrength = 0.5;
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * lightColor;

	// result color
	resultColor = (ambient + diffuse + specular) * objectColor;
	
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}
