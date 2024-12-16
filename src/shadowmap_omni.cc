#include "gllelucamera.hh"
#include "gllelu_main.hh"
#include "shader.hh"
#include "shapes.hh"
#include "texture.hh"

#include "glad/gl.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "SDL.h"

#include <array>
#include <cmath>
#include <stdexcept>

#define SHADOWMAP_SIZE 2048

const float floorPlane[] = {
    -1.0f, 0.0f, -1.0f,   0.0f, 1.0f, 0.0f,   0.0f, 5.0f,
     1.0f, 0.0f,  1.0f,   0.0f, 1.0f, 0.0f,   5.0f, 0.0f,
     1.0f, 0.0f, -1.0f,   0.0f, 1.0f, 0.0f,   5.0f, 5.0f,
     1.0f, 0.0f,  1.0f,   0.0f, 1.0f, 0.0f,   5.0f, 0.0f,
    -1.0f, 0.0f, -1.0f,   0.0f, 1.0f, 0.0f,   0.0f, 5.0f,
    -1.0f, 0.0f,  1.0f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f
};

class ShadowMapOmni: public GLleluCamera
{
public:
    ShadowMapOmni(int argc, char *argv[]);
    virtual ~ShadowMapOmni();

protected:
    virtual Status event(SDL_Event &event);
    virtual Status render();
    void drawScene(Shader &shader);

private:
    Shader m_shadowShader;
    Shader m_sceneShader;
    Texture m_floorTexture;
    Texture m_cubeTexture;
    Texture m_cubeSpecularTexture;
    unsigned int m_planeVAO;
    unsigned int m_planeVBO;
    unsigned int m_cubeVAO;
    unsigned int m_cubeVBO;
    std::array<unsigned int, 6> m_shadowMapFBOs;
    unsigned int m_shadowMap;
    bool m_shadowMapping;
};

ShadowMapOmni::ShadowMapOmni(int argc, char *argv[]):
    GLleluCamera(argc, argv),
    m_shadowShader("shadowmap_omni.vert", "shadowmap_omni.frag"),
    m_sceneShader("shadowmap_omni_scene.vert", "shadowmap_omni_scene.frag"),
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
    camera().position = glm::vec3(-2.5f, 3.5f, 4.5f);
    camera().pitch = -40.0f;
    camera().yaw = -60.0f;

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

    glGenTextures(1, &m_shadowMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_shadowMap);

    for (int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOWMAP_SIZE, SHADOWMAP_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    for (int i = 0; i < 6; ++i) {
        glGenFramebuffers(1, &m_shadowMapFBOs[i]);
        glBindFramebuffer(GL_FRAMEBUFFER, m_shadowMapFBOs[i]);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_shadowMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            throw std::runtime_error("Failed to create FBO");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

ShadowMapOmni::~ShadowMapOmni()
{
    glDeleteVertexArrays(1, &m_planeVAO);
    glDeleteBuffers(1, &m_planeVBO);
    glDeleteVertexArrays(1, &m_cubeVAO);
    glDeleteBuffers(1, &m_cubeVBO);
    glDeleteFramebuffers(static_cast<int>(m_shadowMapFBOs.size()), m_shadowMapFBOs.data());
    glDeleteTextures(1, &m_shadowMap);
}

Status ShadowMapOmni::event(SDL_Event &event)
{
    switch (event.type) {
        case SDL_KEYUP:
            if (SDL_SCANCODE_RETURN == event.key.keysym.scancode) {
                m_shadowMapping = !m_shadowMapping;
            }
            break;
        default:
            break;
    }
    return Status::Ok;
}

Status ShadowMapOmni::render()
{
    glm::vec3 lightPos = glm::vec3(4 * sin(static_cast<float>(SDL_GetTicks()) / 1000.0f), 1.0f, 0.0f);
    float aspect = 1.0f;
    float near = 1.0f;
    float far = 25.0f;
    glm::mat4 lightProjection = glm::perspective(glm::radians(90.0f), aspect, near, far);
    std::array<glm::mat4, 6> lightSpaceMatrices = {
        lightProjection * glm::lookAt(lightPos, lightPos + glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        lightProjection * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        lightProjection * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        lightProjection * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        lightProjection * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        lightProjection * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    if (m_shadowMapping) {
        glViewport(0, 0, SHADOWMAP_SIZE, SHADOWMAP_SIZE);

        m_shadowShader.use();
        m_shadowShader.setVec3("lightPos", lightPos);
        m_shadowShader.setFloat("farPlane", far);

        for (int i = 0; i < 6; ++i) {
            glBindFramebuffer(GL_FRAMEBUFFER, m_shadowMapFBOs[i]);
            glClear(GL_DEPTH_BUFFER_BIT);

            m_shadowShader.setMat4("lightSpaceMat", lightSpaceMatrices[i]);
            drawScene(m_shadowShader);
        }
    }

    glViewport(0, 0, fbSize().width, fbSize().height);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_sceneShader.use();
    m_sceneShader.setBool("enableShadows", m_shadowMapping);
    m_sceneShader.setInt("shadowMap", 10);
    m_sceneShader.setFloat("farPlane", far);

    m_sceneShader.setMat4("view", view());
    m_sceneShader.setMat4("projection", projection());

    m_sceneShader.setFloat("material.shininess", 64.0f);
    m_sceneShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
    m_sceneShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
    m_sceneShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
    m_sceneShader.setVec3("light.position", lightPos);
    m_sceneShader.setVec3("viewPos", camera().position);

    glActiveTexture(GL_TEXTURE0 + 10);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_shadowMap);

    drawScene(m_sceneShader);

    swapWindow();

    return Status::Ok;
}

void ShadowMapOmni::drawScene(Shader &shader)
{
    shader.setInt("material.diffuse", 0);
    shader.setInt("material.specular", 0);

    m_floorTexture.activate(0);

    glBindVertexArray(m_planeVAO);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f));
    model = glm::scale(model, glm::vec3(5.0f, 1.0f, 5.0f));
    shader.setMat4("model", model);
    shader.setMat4("normalMat", glm::transpose(glm::inverse(model)));
    glDrawArrays(GL_TRIANGLES, 0, 6);

    shader.setInt("material.diffuse", 0);
    shader.setInt("material.specular", 1);

    m_cubeTexture.activate(0);
    m_cubeSpecularTexture.activate(1);

    glBindVertexArray(m_cubeVAO);
    model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 2.0f));
    shader.setMat4("model", model);
    shader.setMat4("normalMat", glm::transpose(glm::inverse(model)));
    glDrawArrays(GL_TRIANGLES, 0, 36);

    model = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, 2.0f));
    shader.setMat4("model", model);
    shader.setMat4("normalMat", glm::transpose(glm::inverse(model)));
    glDrawArrays(GL_TRIANGLES, 0, 36);

    model = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0.0f, 2.0f));
    shader.setMat4("model", model);
    shader.setMat4("normalMat", glm::transpose(glm::inverse(model)));
    glDrawArrays(GL_TRIANGLES, 0, 36);

    model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.0f));
    shader.setMat4("model", model);
    shader.setMat4("normalMat", glm::transpose(glm::inverse(model)));
    glDrawArrays(GL_TRIANGLES, 0, 36);

    model = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, -2.0f));
    shader.setMat4("model", model);
    shader.setMat4("normalMat", glm::transpose(glm::inverse(model)));
    glDrawArrays(GL_TRIANGLES, 0, 36);

    model = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0.0f, -2.0f));
    shader.setMat4("model", model);
    shader.setMat4("normalMat", glm::transpose(glm::inverse(model)));
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

GLLELU_MAIN_IMPLEMENTATION(ShadowMapOmni)
