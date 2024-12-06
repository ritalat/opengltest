#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 TangentFragPos;
in vec3 TangentLightPos;
in vec3 TangentViewPos;

struct Material {
    sampler2D diffuse;
    sampler2D normal;
    float shininess;
};

uniform Material material;

void main()
{
    vec3 ambient = 0.1 * vec3(texture(material.diffuse, TexCoord));

    vec3 normal = vec3(texture(material.normal, TexCoord));
    normal = normalize(normal * 2.0 - 1.0);

    vec3 lightDir = normalize(TangentLightPos - TangentFragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * vec3(texture(material.diffuse, TexCoord));

    vec3 viewDir = normalize(TangentViewPos - TangentFragPos);
    vec3 halfDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfDir), 0.0), material.shininess);
    vec3 specular = vec3(0.2) * spec;

    vec3 phong = ambient + diffuse + specular;
    FragColor = vec4(phong, 1.0);
}
