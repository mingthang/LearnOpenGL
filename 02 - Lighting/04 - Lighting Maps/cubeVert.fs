#version 460 core

in vec3 FragColor;
in vec2 TexCoords;

out vec4 FragOutput;

uniform sampler2D material_diffuse; // Texture sampler

void main()
{
    // Sample the texture in the fragment shader
    vec3 texColor = texture(material_diffuse, TexCoords).rgb;

    // Apply texture color
    FragOutput = vec4(FragColor * texColor, 1.0);
}
