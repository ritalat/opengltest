#include "camera.hh"
#include "gllelu.hh"
#include "gllelu_main.hh"
#include "shader.hh"
#include "shapes.hh"
#include "texture.hh"

#include "glad/gl.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "SDL3/SDL.h"

#include <cmath>
#include <stdexcept>

#define SHADOWMAP_SIZE 2048

const float floorPlane[] = {
    -1.0f, 0.0f, -1.0f,   0.0f, 1.0f, 0.0f,   0.0f, 2.5f,
     1.0f, 0.0f,  1.0f,   0.0f, 1.0f, 0.0f,   2.5f, 0.0f,
     1.0f, 0.0f, -1.0f,   0.0f, 1.0f, 0.0f,   2.5f, 2.5f,
     1.0f, 0.0f,  1.0f,   0.0f, 1.0f, 0.0f,   2.5f, 0.0f,
    -1.0f, 0.0f, -1.0f,   0.0f, 1.0f, 0.0f,   0.0f, 2.5f,
    -1.0f, 0.0f,  1.0f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f
};

class ShadowMap: public GLlelu
{
public:
    ShadowMap(int argc, char *argv[]);
    virtual ~ShadowMap();
    virtual SDL_AppResult event(SDL_Event *event);
    virtual SDL_AppResult iterate();
    void drawScene(Shader &shader);

private:
    Camera m_camera;
    Shader m_shadowShader;
    Shader m_sceneShader;
    Texture m_floorTexture;
    Texture m_cubeTexture;
    Texture m_cubeSpecularTexture;
    unsigned int m_planeVAO;
    unsigned int m_planeVBO;
    unsigned int m_cubeVAO;
    unsigned int m_cubeVBO;
    unsigned int m_shadowMapFBO;
    unsigned int m_shadowMap;
    bool m_shadowMapping;
};

ShadowMap::ShadowMap(int argc, char *argv[]):
    GLlelu(argc, argv),
    m_camera(this),
    m_shadowShader("shadowmap.vert", "shadowmap.frag"),
    m_sceneShader("shadowmap_scene.vert", "shadowmap_scene.frag"),
    m_floorTexture("lgl_wall.jpg"),
    m_cubeTexture("lgl_container2.png"),
    m_cubeSpecularTexture("lgl_container2_specular.png"),
    m_planeVAO(0),
    m_planeVBO(0),
    m_cubeVAO(0),
    m_cubeVBO(0),
    m_shadowMap(0),
    m_shadowMapping(true)
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    m_camera.data.position = glm::vec3(1.5f, 5.5f, 3.0f);
    m_camera.data.pitch = -60.0f;
    m_camera.data.yaw = -120.0f;

    glGenVertexArrays(1, &m_planeVAO);
    glBindVertexArray(m_planeVAO);

    glGenBuffers(1, &m_planeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floorPlane), floorPlane, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glGenVertexArrays(1, &m_cubeVAO);
    glBindVertexArray(m_cubeVAO);

    glGenBuffers(1, &m_cubeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenFramebuffers(1, &m_shadowMapFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_shadowMapFBO);

    glGenTextures(1, &m_shadowMap);
    glBindTexture(GL_TEXTURE_2D, m_shadowMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOWMAP_SIZE, SHADOWMAP_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_shadowMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        throw std::runtime_error("Failed to create FBO");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

ShadowMap::~ShadowMap()
{
    glDeleteVertexArrays(1, &m_planeVAO);
    glDeleteBuffers(1, &m_planeVBO);
    glDeleteVertexArrays(1, &m_cubeVAO);
    glDeleteBuffers(1, &m_cubeVBO);
    glDeleteFramebuffers(1, &m_shadowMapFBO);
    glDeleteTextures(1, &m_shadowMap);
}

SDL_AppResult ShadowMap::event(SDL_Event *event)
{
    switch (event->type) {
        case SDL_EVENT_KEY_UP:
            if (SDL_SCANCODE_RETURN == event->key.scancode) {
                m_shadowMapping = !m_shadowMapping;
            }
            break;
        default:
            break;
    }

    m_camera.event(event);
    return GLlelu::event(event);
}

SDL_AppResult ShadowMap::iterate()
{
    m_camera.iterate();

    glm::vec3 lightPos = glm::vec3(-2.0f * sin(static_cast<float>(SDL_GetTicks()) / 500.0f),
                                   4.0f,
                                   2.0f * cos(static_cast<float>(SDL_GetTicks()) / 500.0f));
    glm::vec3 lightTarget = glm::vec3(0.0f);
    glm::mat4 lightProjection = glm::ortho(-10.f, 10.0f, -10.0f, 10.0f, 1.0f, 7.5f);
    glm::mat4 lightView = glm::lookAt(lightPos,
                                      lightTarget,
                                      glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 lightSpaceMat = lightProjection * lightView;

    if (m_shadowMapping) {
        glViewport(0, 0, SHADOWMAP_SIZE, SHADOWMAP_SIZE);
        glBindFramebuffer(GL_FRAMEBUFFER, m_shadowMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);

        m_shadowShader.use();
        m_shadowShader.setMat4("lightSpaceMat", lightSpaceMat);
        glDisable(GL_CULL_FACE);
        drawScene(m_shadowShader);
        glEnable(GL_CULL_FACE);
    }

    glViewport(0, 0, fbSize().width, fbSize().height);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_sceneShader.use();
    m_sceneShader.setBool("enableShadows", m_shadowMapping);
    m_sceneShader.setInt("shadowMap", 10);

    m_sceneShader.setMat4("view", m_camera.view());
    m_sceneShader.setMat4("projection", m_camera.projection());
    m_sceneShader.setMat4("lightSpaceMat", lightSpaceMat);

    m_sceneShader.setFloat("material.shininess", 64.0f);
    m_sceneShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
    m_sceneShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
    m_sceneShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
    m_sceneShader.setVec3("light.direction", lightTarget - lightPos);
    m_sceneShader.setVec3("viewPos", m_camera.data.position);

    glActiveTexture(GL_TEXTURE0 + 10);
    glBindTexture(GL_TEXTURE_2D, m_shadowMap);

    drawScene(m_sceneShader);

    return GLlelu::iterate();
}

void ShadowMap::drawScene(Shader &shader)
{
    shader.setInt("material.diffuse", 0);
    shader.setInt("material.specular", 0);

    m_floorTexture.activate(0);

    glBindVertexArray(m_planeVAO);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f));
    model = glm::scale(model, glm::vec3(2.5f, 1.0f, 2.5f));
    shader.setMat4("model", model);
    shader.setMat4("normalMat", glm::transpose(glm::inverse(model)));
    glDrawArrays(GL_TRIANGLES, 0, 6);

    shader.setInt("material.diffuse", 0);
    shader.setInt("material.specular", 1);

    m_cubeTexture.activate(0);
    m_cubeSpecularTexture.activate(1);

    glBindVertexArray(m_cubeVAO);
    model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.0f, 1.0f));
    shader.setMat4("model", model);
    shader.setMat4("normalMat", glm::transpose(glm::inverse(model)));
    glDrawArrays(GL_TRIANGLES, 0, 36);

    model = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, -1.0f));
    shader.setMat4("model", model);
    shader.setMat4("normalMat", glm::transpose(glm::inverse(model)));
    glDrawArrays(GL_TRIANGLES, 0, 36);

    model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.0f, 0.0f));
    model = glm::rotate(model,
                        glm::radians(static_cast<float>(SDL_GetTicks()) / 10.0f),
                        glm::vec3(0.0f, 1.0f, 0.0f));
    shader.setMat4("model", model);
    shader.setMat4("normalMat", glm::transpose(glm::inverse(model)));
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

GLLELU_MAIN_IMPLEMENTATION(ShadowMap)
