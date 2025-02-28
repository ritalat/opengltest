#include "gllelu.hh"
#include "gllelu_main.hh"
#include "shader.hh"

#include "glad/gl.h"
#include "glm/glm.hpp"
#include "SDL3/SDL.h"

#include <cstdint>
#include <cstdlib>

#define NUM_PARTICLES 1000
#define LOCAL_SIZE_X 100

struct Particle {
    glm::vec2 position;
    glm::vec2 velocity;
};

struct Uniforms {
    float dt;
    float scale;
};

class ComputeParticles: public GLlelu
{
public:
    ComputeParticles(int argc, char *argv[]);
    virtual ~ComputeParticles();
    virtual SDL_AppResult event(SDL_Event *event);
    virtual SDL_AppResult iterate();

private:
    Shader m_computeShader;
    Shader m_drawShader;
    Particle m_initialParticles[NUM_PARTICLES];
    unsigned int m_dummyVAO;
    unsigned int m_particleBufferA;
    unsigned int m_particleBufferB;
    unsigned int m_UBO;
    uint64_t m_frameCount;
    uint64_t m_lastFrame;
    int m_speedScale;
};

ComputeParticles::ComputeParticles(int argc, char *argv[]):
    GLlelu(argc, argv, GLVersion::GL43),
    m_computeShader("compute_particles.comp"),
    m_drawShader("compute_particles.vert", "compute_particles.frag"),
    m_dummyVAO(0),
    m_particleBufferA(0),
    m_particleBufferB(0),
    m_UBO(0),
    m_frameCount(0),
    m_lastFrame(0),
    m_speedScale(0)
{
    for (int i = 0; i < NUM_PARTICLES; ++i) {
        Particle p {
            glm::vec2(0.0f, 0.0f),
            glm::vec2(static_cast<float>(rand()) / static_cast<float>(RAND_MAX),
                      static_cast<float>(rand()) / static_cast<float>(RAND_MAX))
        };
        p.velocity.x = p.velocity.x * 2.0f - 1.0f;
        p.velocity.y = p.velocity.y * 2.0f - 1.0f;
        p.velocity = glm::normalize(p.velocity);
        m_initialParticles[i] = p;
    }

    glGenVertexArrays(1, &m_dummyVAO);

    glGenBuffers(1, &m_particleBufferA);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_particleBufferA);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(m_initialParticles), m_initialParticles, GL_DYNAMIC_COPY);

    glGenBuffers(1, &m_particleBufferB);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_particleBufferB);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(m_initialParticles), NULL, GL_DYNAMIC_COPY);

    glGenBuffers(1, &m_UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, m_UBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(Uniforms), NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_UBO);

    glEnable(GL_PROGRAM_POINT_SIZE);
}

ComputeParticles::~ComputeParticles()
{
    glDeleteVertexArrays(1, &m_dummyVAO);
    glDeleteBuffers(1, &m_particleBufferA);
    glDeleteBuffers(1, &m_particleBufferB);
    glDeleteBuffers(1, &m_UBO);
}

SDL_AppResult ComputeParticles::event(SDL_Event *event)
{
    switch (event->type) {
        case SDL_EVENT_KEY_UP:
            switch (event->key.scancode) {
                case SDL_SCANCODE_RETURN:
                    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_particleBufferA);
                    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(m_initialParticles), m_initialParticles);
                    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_particleBufferB);
                    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(m_initialParticles), m_initialParticles);
                    break;
                case SDL_SCANCODE_UP:
                    ++m_speedScale;
                    break;
                case SDL_SCANCODE_DOWN:
                    --m_speedScale;
                default:
                    break;
            }
            break;
        default:
            break;
    }

    return GLlelu::event(event);
}

SDL_AppResult ComputeParticles::iterate()
{
    uint64_t currentFrame = SDL_GetTicks();
    uint64_t delta = currentFrame - m_lastFrame;
    m_lastFrame = currentFrame;

    Uniforms uniforms {
        static_cast<float>(delta) / 1000.0f,
        1.0f + static_cast<float>(m_speedScale) / 10.0f
    };

    if (m_frameCount % 2 == 0) {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_particleBufferA);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_particleBufferB);
    } else {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_particleBufferA);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_particleBufferB);
    }

    m_computeShader.use();
    glBindBuffer(GL_UNIFORM_BUFFER, m_UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Uniforms), &uniforms);

    glDispatchCompute(NUM_PARTICLES / LOCAL_SIZE_X, 1, 1);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    m_drawShader.use();
    glBindVertexArray(m_dummyVAO);
    glDrawArrays(GL_POINTS, 0, NUM_PARTICLES);

    ++m_frameCount;

    return GLlelu::iterate();
}

GLLELU_MAIN_IMPLEMENTATION(ComputeParticles)
