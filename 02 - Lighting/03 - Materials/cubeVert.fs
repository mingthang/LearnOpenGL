// Per-vertex Shading (Gouraud Shading)
#version 460 core

out vec4 FragColor;

in vec3 resultColor;

void main()
{
    FragColor = vec4(resultColor, 1.0);
}
