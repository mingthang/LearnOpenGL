// Per-Fragment Shading (Phong Shading)
#version 460 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

struct Material
{
    sampler2D diffuse;
	sampler2D specular;
    sampler2D emission;
    sampler2D gradient;
	float shininess;
};

struct Light
{
	vec3 position;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform vec3 viewPos; 
uniform Material material;
uniform Light light;
uniform float time;

void main()
{
    // ambient
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
    
    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), float(material.shininess));
    vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;

    // emission
    vec3 show = step(vec3(1.0), vec3(1.0) - texture(material.specular, TexCoords).rgb);
    //vec3 emission = texture(material.emission, TexCoords + vec2(0.0, time)).rgb;
    vec3 emission = texture(material.emission, TexCoords).rgb;
    vec3 colorShift = texture(material.gradient, vec2(fract(time * 0.1), 0.5)).rgb;
    emission *= colorShift;
    emission *= show;
    //emission *= (sin(time) * 0.5 + 0.5) * 2.0;
    
    vec3 result = ambient + diffuse + specular + emission;
    FragColor = vec4(result, 1.0f);
}