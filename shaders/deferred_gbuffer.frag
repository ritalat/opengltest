#version 330 core
layout (location = 0) out vec3 gposition;
layout (location = 1) out vec3 gnormal;
layout (location = 2) out vec4 gcolor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform sampler2D diffuse;
uniform sampler2D specular;

void main()
{
    gposition = FragPos;
    gnormal = normalize(Normal);
    gcolor.rgb = texture(diffuse, TexCoord).rgb;
    gcolor.a = texture(specular, TexCoord).r;
}
