#version 330 core

layout (location = 0) in vec3 inPosition;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

out vec3 FragPos;

void main()
{
    vec4 worldPos = model * vec4(inPosition, 1.0);
    FragPos = vec3(worldPos);
    gl_Position = lightSpaceMatrix * worldPos;
}
