#version 330 core

in VertexData {
    vec3 fragPos;
    vec3 normal;
    vec2 texCoord;
} fsIn;

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

uniform Material material;
uniform Light light;
uniform vec3 viewPos;

out vec4 FragColor;

void main()
{
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, fsIn.texCoord));

    vec3 norm = normalize(fsIn.normal);
    vec3 lightDir = normalize(light.position - fsIn.fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, fsIn.texCoord));

    vec3 viewDir = normalize(viewPos - fsIn.fragPos);
    vec3 halfDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, fsIn.texCoord));

    vec3 phong = ambient + diffuse + specular;
    FragColor = vec4(phong, 1.0);
}
