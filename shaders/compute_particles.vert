#version 430 core

struct Particle {
    vec2 position;
    vec2 velocity;
};

layout(std430, binding = 1) readonly buffer ParticleBuffer {
    Particle particles[];
};

void main()
{
    gl_PointSize = 2.0;
    gl_Position = vec4(particles[gl_VertexID].position, 0.0, 1.0);
}
