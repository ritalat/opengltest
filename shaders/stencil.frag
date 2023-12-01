#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture0;

uniform float alpha;

void main()
{
    vec4 color = texture(texture0, TexCoord);
    FragColor = vec4(color.rgb, alpha);
}
