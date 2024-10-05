#include "gllelucamera.hh"
#include "gllelu_main.hh"
#include "path.hh"
#include "shader.hh"
#include "shapes.hh"
#include "texture.hh"

#include "glad/gl.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "SDL3/SDL.h"

#include <cstdio>
#include <cstdlib>

const glm::vec3 cubePositions[] = {
    glm::vec3( 0.0f,  0.0f,  0.0f),
    glm::vec3( 2.0f,  5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3( 2.4f, -0.4f, -3.5f),
    glm::vec3(-1.7f,  3.0f, -7.5f),
    glm::vec3( 1.3f, -2.0f, -2.5f),
    glm::vec3( 1.5f,  2.0f, -2.5f),
    glm::vec3( 1.5f,  0.2f, -1.5f),
    glm::vec3(-1.3f,  1.0f, -1.5f)
};

const glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

class LGL2Textured: public GLleluCamera
{
public:
    LGL2Textured(int argc, char *argv[]);
    virtual ~LGL2Textured();

protected:
    virtual Status render();

private:
    Shader m_lightingShader;
    Shader m_lightShader;
    Texture m_diffuseMap;
    Texture m_specularMap;
    unsigned int m_VAO;
    unsigned int m_lightVAO;
    unsigned int m_VBO;
};

LGL2Textured::LGL2Textured(int argc, char *argv[]):
    GLleluCamera(argc, argv),
    m_lightingShader("lighting.vert", "lighting_textured.frag"),
    m_lightShader("light.vert", "light.frag"),
    m_diffuseMap("lgl_container2.png"),
    m_specularMap("lgl_container2_specular.png"),
    m_VAO(0),
    m_lightVAO(0),
    m_VBO(0)
{
    m_lightingShader.use();
    m_lightingShader.setInt("material.diffuse", 0);
    m_lightingShader.setInt("material.specular", 1);

    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &m_lightVAO);
    glBindVertexArray(m_lightVAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

LGL2Textured::~LGL2Textured()
{
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteVertexArrays(1, &m_lightVAO);
    glDeleteBuffers(1, &m_VBO);
}

Status LGL2Textured::render()
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_diffuseMap.activate(0);
    m_specularMap.activate(1);

    m_lightingShader.use();

    m_lightingShader.setMat4("view", view());
    m_lightingShader.setMat4("projection", projection());

    m_lightingShader.setFloat("material.shininess", 64.0f);
    m_lightingShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
    m_lightingShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
    m_lightingShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
    m_lightingShader.setVec3("light.position", lightPos);
    m_lightingShader.setVec3("viewPos", camera().position);

    glm::mat4 model = glm::mat4(1.0f);
    m_lightingShader.setMat4("model", model);
    m_lightingShader.setMat4("normalMat", glm::transpose(glm::inverse(model)));

    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    m_lightShader.use();

    m_lightShader.setMat4("view", view());
    m_lightShader.setMat4("projection", projection());

    model = glm::mat4(1.0f);
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.2f));
    m_lightShader.setMat4("model", model);

    glBindVertexArray(m_lightVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    swapWindow();

    return Status::Ok;
}

GLLELU_MAIN_IMPLEMENTATION(LGL2Textured)
