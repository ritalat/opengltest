#version 330 core

in vec3 FragPos;

uniform vec3 lightPos;
uniform float farPlane;

void main()
{
    float lightDist = length(FragPos - lightPos);
    gl_FragDepth = lightDist / farPlane;
}
