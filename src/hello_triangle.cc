#include "gllelu.hh"
#include "gllelu_main.hh"
#include "shader.hh"

#include "glad/gl.h"
#include "SDL3/SDL.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <string>

const float vertices[] = {
     0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
     0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f
};

class HelloTriangle: public GLlelu
{
public:
    HelloTriangle(int argc, char *argv[]);
    virtual ~HelloTriangle();
    virtual SDL_AppResult iterate();

private:
    Shader m_triangleShader;
    unsigned int m_VAO;
    unsigned int m_VBO;
};

HelloTriangle::HelloTriangle(int argc, char *argv[]):
    GLlelu(argc, argv),
    m_triangleShader("triangle.vert", "triangle.frag"),
    m_VAO(0),
    m_VBO(0)
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

}

HelloTriangle::~HelloTriangle()
{
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
}

SDL_AppResult HelloTriangle::iterate()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    m_triangleShader.use();

    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    return GLlelu::iterate();
}

GLLELU_MAIN_IMPLEMENTATION(HelloTriangle)
