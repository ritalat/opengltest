#version 330 core

in VertexData {
    vec3 fragPos;
    vec3 normal;
    vec2 texCoord;
} fsIn;

uniform sampler2D diffuse;
uniform sampler2D specular;

layout (location = 0) out vec3 gposition;
layout (location = 1) out vec3 gnormal;
layout (location = 2) out vec4 gcolor;

void main()
{
    gposition = fsIn.fragPos;
    gnormal = normalize(fsIn.normal);
    gcolor.rgb = texture(diffuse, fsIn.texCoord).rgb;
    gcolor.a = texture(specular, fsIn.texCoord).r;
}
