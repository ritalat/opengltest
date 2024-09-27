#version 300 es
precision highp float;

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D font;
uniform vec3 color;

void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(font, TexCoord).r);
    FragColor = vec4(color, 1.0) * sampled;
}
