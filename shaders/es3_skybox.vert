#version 300 es

layout (location = 0) in vec3 inPosition;

uniform mat4 view;
uniform mat4 projection;

out vec3 TexCoord;

void main()
{
    TexCoord = inPosition;
    vec4 pos = projection * view * vec4(inPosition, 1.0);
    gl_Position = pos.xyww;
}
