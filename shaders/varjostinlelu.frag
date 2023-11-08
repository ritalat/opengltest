#version 330 core
out vec4 FragColor;

uniform vec2 u_resolution;
uniform vec2 u_mouse;
uniform float u_time;

void main()
{
    vec2 uv = gl_FragCoord.xy / u_resolution;
    vec3 color = 0.5 + 0.5 * cos(u_time + uv.xyx + vec3(0.0, 2.0, 4.0));
    FragColor = vec4(color, 1.0);
}
