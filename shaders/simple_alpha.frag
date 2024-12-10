#version 330 core

in vec2 TexCoord;

uniform sampler2D texture0;

out vec4 FragColor;

void main()
{
    vec4 color = texture(texture0, TexCoord);
    if (color.a < 0.1)
        discard;
    FragColor = color;
}
