#version 330 core

in vec3 FragPos;
in vec3 Normal;

uniform samplerCube skybox;
uniform vec3 viewPos;

out vec4 FragColor;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(FragPos - viewPos);
    vec3 reflectDir = reflect(viewDir, norm);
    FragColor = texture(skybox, reflectDir);
}
