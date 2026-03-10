#version 460 core

#define NR_POINT_LIGHTS 3
#define NR_TEXTURE_MAPS 3

struct Material {
    sampler2D diffuse[NR_TEXTURE_MAPS];
    sampler2D specular[NR_TEXTURE_MAPS];
    vec3 ambientVec;
    vec3 diffuseVec;
    vec3 specularVec;
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

float calcSpec(vec3 lightDir, vec3 normal, vec3 view) {
    vec3 reflectDir = reflect(-lightDir, normal);
    return pow(max(dot(view, reflectDir), 0.0), material.shininess);
}

vec3 calculateLight(DirectionalLight light, float diff, float spec, vec3 diffuseTotal, vec3 specTotal) {
    vec3 ambient = light.ambient * diffuseTotal; //+ material.ambientVec; 
    vec3 diffuse = light.diffuse * diff * diffuseTotal; 
    vec3 specular = light.specular * spec * specTotal;
    return (ambient + diffuse + specular);
}

vec3 calcDirLight(DirectionalLight light, vec3 normal, vec3 view, vec3 diffuseTotal, vec3 specTotal) {
    vec3 lightDir = normalize(light.direction);
    float diff = max(dot(normal, lightDir), 0.0);

    float spec = calcSpec(lightDir, normal, view);
    
    return calculateLight(light, diff, spec, diffuseTotal, specTotal);
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 view, vec3 diffuseTotal, vec3 specTotal) {
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);

    float spec = calcSpec(lightDir, normal, view);

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
   
    vec3 strength = calculateLight(DirectionalLight(vec3(0), light.ambient, light.diffuse, light.specular), diff, spec, diffuseTotal, specTotal); 
    strength *= attenuation;
    return strength;
}

void main()
{
    vec3 norm = normalize(voNormal);
    vec3 viewDir = normalize(viewPos - voFragPos);
    vec3 diffuseTotal = vec3(0);
    vec3 specTotal = vec3(0);
    for (int i = 0; i < NR_TEXTURE_MAPS; i++) {
        diffuseTotal += texture(material.diffuse[i], voTexCoords).rgb; 
        specTotal += texture(material.specular[i], voTexCoords).rgb; 
    }/*
    diffuseTotal += material.diffuseVec;
    specTotal += material.specularVec;
*/
    vec3 result = calcDirLight(dirLight, norm, viewDir, diffuseTotal, specTotal);
    for (int i = 0; i < NR_POINT_LIGHTS; i++)
        result += calcPointLight(pointLights[i], norm, voFragPos, viewDir, diffuseTotal, specTotal);
    
    FragColor = vec4(result, 1.0);
} 
