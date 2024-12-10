#version 330 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoord;
layout (location = 3) in vec3 inTangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPos;
uniform vec3 viewPos;

out VertexData {
    vec2 texCoord;
    vec3 tangentFragPos;
    vec3 tangentLightPos;
    vec3 tangentViewPos;
} vsOut;

void main()
{
    vec4 worldPos = model * vec4(inPosition, 1.0);
    vsOut.texCoord = inTexCoord;
    vec3 bitangent = cross(inNormal, inTangent);
    vec3 T = normalize(vec3(model * vec4(inTangent, 0.0)));
    vec3 B = normalize(vec3(model * vec4(bitangent, 0.0)));
    vec3 N = normalize(vec3(model * vec4(inNormal, 0.0)));
    mat3 TBN = transpose(mat3(T, B, N));
    vsOut.tangentFragPos = TBN * vec3(worldPos);
    vsOut.tangentLightPos = TBN * lightPos;
    vsOut.tangentViewPos = TBN * viewPos;
    gl_Position = projection * view * worldPos;
}
