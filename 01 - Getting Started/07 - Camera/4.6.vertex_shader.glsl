#version 460 core
layout (location = 0) in vec3 aPos;

out vec3 aColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);

    // Map aPos.y [0, 6] to the range [0, 255]
    float mappedY = (aPos.y) ;
    aColor = vec3(mappedY);
}
