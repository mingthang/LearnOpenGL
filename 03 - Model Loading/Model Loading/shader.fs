#version 460 core

struct DirLight
{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define NR_POINT_LIGHTS 3
struct PointLight
{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    // attenuation
    float constant;
    float linear;
    float quadratic;
};

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform float shininess;

vec3 CalDirLight(DirLight dirLight, vec3 norm, vec3 viewDir);
vec3 CalPointLight(PointLight pointLight, vec3 norm, vec3 FragPos, vec3 viewDir);

void main()
{    
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // direction light
    vec3 result = CalDirLight(dirLight, norm, viewDir);
    // point lights
    for(int i = 0; i < NR_POINT_LIGHTS; ++i)
        result += CalPointLight(pointLights[i], norm, FragPos, viewDir);

    FragColor = vec4(result, 1.0); 
}

vec3 CalDirLight(DirLight dirLight, vec3 norm, vec3 viewDir)
{
    // Ambient component
    vec3 ambient = dirLight.ambient * texture(texture_diffuse1, TexCoords).rgb;
    
    // Diffuse component
    vec3 lightDir = normalize(-dirLight.direction);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = dirLight.diffuse * diff * texture(texture_diffuse1, TexCoords).rgb;

    // Specular component
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(reflectDir, viewDir), 0.0), shininess);
    vec3 specular = dirLight.specular * spec * texture(texture_specular1, TexCoords).rgb;

    return ambient + diffuse + specular;
}

vec3 CalPointLight(PointLight pointLight, vec3 norm, vec3 FragPos, vec3 viewDir)
{
    vec3 ambient = pointLight.ambient * texture(texture_diffuse1, TexCoords).rgb;

    vec3 lightDir = normalize(pointLight.position - FragPos);
    float diff = max(dot(lightDir, norm), 0.0);
    vec3 diffuse = pointLight.diffuse * diff * texture(texture_diffuse1, TexCoords).rgb;

    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(reflectDir, viewDir), 0.0), shininess);
    vec3 specular = pointLight.specular * spec * texture(texture_specular1, TexCoords).rgb;

    float distance = length(pointLight.position - FragPos);
    float attenuation = 1.0 / (pointLight.constant + pointLight.linear * distance + pointLight.quadratic * (distance * distance));

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    vec3 result = ambient + diffuse + specular;
    return result;
}