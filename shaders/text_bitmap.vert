#version 330 core

layout (location = 0) in vec2 inPosition;
layout (location = 1) in vec2 inTexCoord;

uniform mat4 projection;
uniform mat2 texScale;

out vec2 TexCoord;

void main()
{
    TexCoord = texScale * inTexCoord;
    gl_Position = projection * vec4(inPosition, 0.0, 1.0);
}
