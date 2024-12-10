#version 330 core

in VertexData {
    vec2 texCoord;
    vec3 tangentFragPos;
    vec3 tangentLightPos;
    vec3 tangentViewPos;
} fsIn;

struct Material {
    sampler2D diffuse;
    sampler2D normal;
    float shininess;
};

uniform Material material;

out vec4 FragColor;

void main()
{
    vec3 ambient = 0.1 * vec3(texture(material.diffuse, fsIn.texCoord));

    vec3 normal = vec3(texture(material.normal, fsIn.texCoord));
    normal = normalize(normal * 2.0 - 1.0);

    vec3 lightDir = normalize(fsIn.tangentLightPos - fsIn.tangentFragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * vec3(texture(material.diffuse, fsIn.texCoord));

    vec3 viewDir = normalize(fsIn.tangentViewPos - fsIn.tangentFragPos);
    vec3 halfDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfDir), 0.0), material.shininess);
    vec3 specular = vec3(0.2) * spec;

    vec3 phong = ambient + diffuse + specular;
    FragColor = vec4(phong, 1.0);
}
