#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define DBG false

uniform Material material;
uniform Light light;
uniform vec3 viewPos;
uniform float farPlane;
uniform samplerCube shadowMap;
uniform bool enableShadows;

float map_shadow()
{
    vec3 lightDir = FragPos - light.position;
    float closestDepth = texture(shadowMap, lightDir).r * farPlane;
    float currentDepth = length(lightDir);

    if (DBG) {
        FragColor = vec4(vec3(closestDepth / farPlane), 1.0);
    }

    float shadowBias = 0.05;
    return currentDepth - shadowBias > closestDepth ? 1.0 : 0.0;
}

void main()
{
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));

    if (DBG) {
        map_shadow();
        return;
    }

    float shadow = enableShadows ? map_shadow() : 0.0;

    vec3 phong = ambient + (1.0 - shadow) * (diffuse + specular);
    FragColor = vec4(phong, 1.0);
}
