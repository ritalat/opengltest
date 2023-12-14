#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform samplerCube skybox;
uniform vec3 viewPos;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(FragPos - viewPos);
    vec3 reflectDir = reflect(viewDir, norm);
    FragColor = texture(skybox, reflectDir);
}
