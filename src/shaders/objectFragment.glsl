#version 460 core

#define NR_POINT_LIGHTS 3
#define NR_TEXTURE_MAPS 3

struct Material {
    sampler2D diffuse[NR_TEXTURE_MAPS];
    sampler2D specular[NR_TEXTURE_MAPS];
    float shininess;
};

struct PointLight {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    // attenuation
    float constant;
    float linear;
    float quadratic;
};

struct DirectionalLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

out vec4 FragColor;

in vec2 voTexCoords;
in vec3 voNormal;
in vec3 voFragPos;

uniform DirectionalLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform Material material;
uniform vec3 viewPos;

vec3 calcDirLight(DirectionalLight light, vec3 normal, vec3 view) {
    vec3 lightDir = normalize(light.direction);
    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(view, reflectDir), 0.0), material.shininess);

    vec3 ambient = light.ambient * vec3(texture(material.diffuse[0], voTexCoords)); 
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse[0], voTexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular[0], voTexCoords)); 
    return (ambient + diffuse + specular);
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 view) {
    vec3 lightDir = normalize(light.position - fragPos); // point light
    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(view, reflectDir), 0.0), material.shininess);

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
    
    vec3 ambient = light.ambient * vec3(texture(material.diffuse[0], voTexCoords)); 
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse[0], voTexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular[0], voTexCoords)); 
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

void main()
{
    vec3 norm = normalize(voNormal);
    vec3 viewDir = normalize(viewPos - voFragPos);
    vec3 result = calcDirLight(dirLight, norm, viewDir);
    for (int i = 0; i < NR_POINT_LIGHTS; i++)
        result += calcPointLight(pointLights[i], norm, voFragPos, viewDir);

    FragColor = vec4(result, 1.0);
} 
