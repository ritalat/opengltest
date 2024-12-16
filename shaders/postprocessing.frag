#version 330 core

in vec2 TexCoord;

uniform sampler2D texture0;
uniform int effect;

out vec4 FragColor;

#define NONE 0
#define NEGATIVE 1
#define GRAYSCALE 2
#define SHARPEN 3
#define BLUR 4
#define EDGE 5

const float offset = 1.0 / 300.0;

vec2 offsets[9] = vec2[](
    vec2(-offset,  offset),
    vec2( 0.0,     offset),
    vec2( offset,  offset),
    vec2(-offset,  0.0   ),
    vec2( 0.0,     0.0   ),
    vec2( offset,  0.0   ),
    vec2(-offset, -offset),
    vec2( 0.0,    -offset),
    vec2( offset, -offset)
);

vec4 kernelEffect(float kernel[9])
{
    vec3 sample[9];
    for (int i = 0; i < 9; ++i) {
        sample[i] = vec3(texture(texture0, TexCoord.st + offsets[i]));
    }
    vec3 color = vec3(0.0);
    for (int i = 0; i < 9; ++i) {
        color += sample[i] * kernel[i];
    }
    return vec4(color, 1.0);
}

vec4 sharpen()
{
    float kernel[9] = float[](
        -1, -1, -1,
        -1,  9, -1,
        -1, -1, -1
    );
    return kernelEffect(kernel);
}

vec4 blur()
{
    float kernel[9] = float[](
        1.0 / 16, 2.0 / 16, 1.0 / 16,
        2.0 / 16, 4.0 / 16, 2.0 / 16,
        1.0 / 16, 2.0 / 16, 1.0 / 16
    );
    return kernelEffect(kernel);
}

vec4 edge()
{
    float kernel[9] = float[](
        1,  1, 1,
        1, -8, 1,
        1,  1, 1
    );
    return kernelEffect(kernel);
}

void main()
{
    vec4 color = vec4(1.0);
    switch (effect) {
        case NEGATIVE:
            color = vec4(1.0 - vec3(texture(texture0, TexCoord)), 1.0);
            break;
        case GRAYSCALE:
            color = texture(texture0, TexCoord);
            float average = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
            color = vec4(average, average, average, 1.0);
            break;
        case SHARPEN:
            color = sharpen();
            break;
        case BLUR:
            color = blur();
            break;
        case EDGE:
            color = edge();
            break;
        case NONE:
        default:
            color = texture(texture0, TexCoord);
            break;
    }
    FragColor = color;
}
