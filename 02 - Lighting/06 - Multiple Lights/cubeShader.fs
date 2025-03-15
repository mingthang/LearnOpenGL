#version 460 core

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct DirLight
{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

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

#define NR_POINT_LIGHTS 4

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform Material material;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];

vec3 CalDirLight(DirLight dirLight, vec3 norm, vec3 viewDir);
vec3 CalPointLight(PointLight pointLight, vec3 norm, vec3 FragPos, vec3 viewDir);

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // direction light
    vec3 result = CalDirLight(dirLight, norm, viewDir);
    // 4 point lights
    for(int i = 0; i < 4; ++i)
        result += CalPointLight(pointLights[i], norm, FragPos, viewDir);

    FragColor = vec4(result, 1.0); 
    
} 

vec3 CalDirLight(DirLight dirLight, vec3 norm, vec3 viewDir)
{
    vec3 ambient = dirLight.ambient * texture(material.diffuse, TexCoords).rgb;
    
    vec3 lightDir = normalize(dirLight.direction);
    float diff = max(dot(lightDir, norm), 0.0);
    vec3 diffuse = dirLight.diffuse * diff * texture(material.diffuse, TexCoords).rgb;

    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(reflectDir, viewDir), 0.0), material.shininess);
    vec3 specular = dirLight.specular * spec * texture(material.specular, TexCoords).rgb;

    vec3 result = ambient + diffuse + specular;
    return result;
}

vec3 CalPointLight(PointLight pointLight, vec3 norm, vec3 FragPos, vec3 viewDir)
{
    vec3 ambient = pointLight.ambient * texture(material.diffuse, TexCoords).rgb;

    vec3 lightDir = normalize(pointLight.position - FragPos);
    float diff = max(dot(lightDir, norm), 0.0);
    vec3 diffuse = pointLight.diffuse * diff * texture(material.diffuse, TexCoords).rgb;

    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(reflectDir, viewDir), 0.0), material.shininess);
    vec3 specular = pointLight.specular * spec * texture(material.specular, TexCoords).rgb;

    float distance = length(pointLight.position - FragPos);
    float attenuation = 1.0 / (pointLight.constant + pointLight.linear * distance + pointLight.quadratic * (distance * distance));

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    vec3 result = ambient + diffuse + specular;
    return result;
}