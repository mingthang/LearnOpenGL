#version 460

layout(location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 projection;

out vec4 aColor;

void main()
{
	gl_Position = projection * model * vec4(aPos, 1.0f);
	aColor = vec4(aPos, 1.0) * 0.5 + vec4(0.5, 0.5, 0.5, 0.5);
}