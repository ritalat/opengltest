#include "gllelu.hh"
#include "gllelu_main.hh"
#include "shader.hh"

#if defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>
#endif
#include "glad/gles2.h"
#include "SDL.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <string>

const float vertices[] = {
     0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
     0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f
};

class TriangleES3: public GLlelu
{
public:
    TriangleES3(int argc, char *argv[]);
    virtual ~TriangleES3();

protected:
    virtual int mainLoop();
    void iterate();

private:
    Shader m_triangleShader;
    unsigned int m_VAO;
    unsigned int m_VBO;
    bool m_quit;
};

#if defined(__EMSCRIPTEN__)
void browserCallback(void *arg)
{
    static_cast<TriangleES3*>(arg)->iterate();
}
#endif

TriangleES3::TriangleES3(int argc, char *argv[]):
    GLlelu(argc, argv),
    m_triangleShader("es3_triangle.vert", "es3_triangle.frag"),
    m_quit(false)
{
}

TriangleES3::~TriangleES3()
{
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
}

int TriangleES3::mainLoop()
{
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

#if defined(__EMSCRIPTEN__)
    emscripten_set_main_loop_arg(browserCallback, this, 0, 1);
#else
    while (!m_quit) {
        iterate();
    }
#endif

    return EXIT_SUCCESS;
}

void TriangleES3::iterate()
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                m_quit = true;
                break;
            case SDL_KEYUP:
                if (SDL_SCANCODE_ESCAPE == event.key.keysym.scancode)
                    m_quit = true;
                break;
            default:
                break;
        }
    }

#if defined(__EMSCRIPTEN__)
    if(m_quit)
        emscripten_cancel_main_loop();
#endif

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    m_triangleShader.use();

    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    swapWindow();
}

GLLELU_MAIN_IMPLEMENTATION(TriangleES3)
