#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

#define NONE 0
#define POSITION 1
#define NORMAL 2
#define COLOR 3
#define SPECULAR 4

struct Light {
    vec4 position;
    vec4 color;
};

#define MAX_LIGHTS 50
#define LINEAR 0.35
#define QUADRATIC 0.44

layout (std140) uniform Lights {
    Light lights[MAX_LIGHTS];
};

uniform sampler2D gposition;
uniform sampler2D gnormal;
uniform sampler2D gcolor;
uniform int numLights;
uniform vec3 viewPos;
uniform int visualizedBuffer;

void main()
{
    vec3 fragPos = texture(gposition, TexCoord).rgb;
    vec3 normal = texture(gnormal, TexCoord).rgb;
    vec3 materialDiffuse = texture(gcolor, TexCoord).rgb;
    float materialSpecular = texture(gcolor, TexCoord).a;
    vec3 lighting = vec3(0.0);

    switch (visualizedBuffer) {
        case POSITION:
            lighting = (fragPos + 1.0) / 2.0;
            break;
        case NORMAL:
            lighting = (normal + 1.0) / 2.0;
            break;
        case COLOR:
            lighting = materialDiffuse;
            break;
        case SPECULAR:
            lighting = vec3(materialSpecular);
            break;
        case NONE:
        default:
            lighting = 0.1 * materialDiffuse;
            vec3 viewDir = normalize(viewPos - fragPos);
            int lightCount = min(numLights, MAX_LIGHTS);

            for (int i = 0; i < lightCount; ++i) {
                vec3 lightDir = normalize(lights[i].position.xyz - fragPos);
                float diff = max(dot(normal, lightDir), 0.0);
                vec3 diffuse = materialDiffuse * diff * lights[i].color.rgb;

                vec3 halfDir = normalize(lightDir + viewDir);
                float spec = pow(max(dot(normal, halfDir), 0.0), 32.0);
                vec3 specular = materialSpecular * spec * lights[i].color.rgb;

                float dist = length(lights[i].position.xyz - fragPos);
                float attenuation = 1.0 / (1.0 + LINEAR * dist + QUADRATIC * dist * dist);
                diffuse *= attenuation;
                specular *= attenuation;
                lighting += diffuse + specular;
            }
            break;
    }
    FragColor = vec4(lighting, 1.0);
}
