#version 460 core

out vec4 FragColor;

in vec2 TexCoords;

layout (binding = 0) uniform sampler2D Texture;

void main() {
    FragColor = texture(Texture, TexCoords);
}