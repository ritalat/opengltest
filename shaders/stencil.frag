#version 330 core

in vec2 TexCoord;

uniform sampler2D texture0;
uniform float alpha;

out vec4 FragColor;

void main()
{
    vec4 color = texture(texture0, TexCoord);
    FragColor = vec4(color.rgb, alpha);
}
