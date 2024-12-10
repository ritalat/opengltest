#version 300 es
precision highp float;

in vec3 TexCoord;

uniform samplerCube skybox;

out vec4 FragColor;

void main()
{
    FragColor = texture(skybox, TexCoord);
}
