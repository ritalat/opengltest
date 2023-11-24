#include "gllelucamera.hh"
#include "gllelu_main.hh"
#include "path.hh"
#include "shader.hh"
#include "shapes.hh"
#include "texture.hh"

#include "glad/gl.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "SDL.h"

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

const glm::vec3 pointLightPositions[] = {
    glm::vec3(0.7f, 0.2f, 2.0f),
    glm::vec3(2.3f, -3.3f, -4.0f),
    glm::vec3(-4.0f, 2.0f, -12.0f),
    glm::vec3(0.0f, 0.0f, -3.0f)
};

class LGL_2: public GLleluCamera
{
public:
    LGL_2(int argc, char *argv[]);
    virtual ~LGL_2();
    virtual Status event(SDL_Event &event);
    virtual Status render();

    Shader m_lightingShader;
    Shader m_lightShader;
    Texture m_diffuseMap;
    Texture m_specularMap;
    unsigned int m_VAO;
    unsigned int m_lightVAO;
    unsigned int m_VBO;
    bool m_enableSpotLight;
};

LGL_2::LGL_2(int argc, char *argv[]):
    GLleluCamera(argc, argv),
    m_lightingShader("lighting.vert", "lighting.frag"),
    m_lightShader("lighting.vert", "lighting_light.frag"),
    m_diffuseMap("lgl_container2.png"),
    m_specularMap("lgl_container2_specular.png"),
    m_VAO(0),
    m_lightVAO(0),
    m_VBO(0),
    m_enableSpotLight(true)
{
    glEnable(GL_DEPTH_TEST);

    m_lightingShader.use();
    m_lightingShader.set_int("material.diffuse", 0);
    m_lightingShader.set_int("material.specular", 1);

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

LGL_2::~LGL_2()
{
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteVertexArrays(1, &m_lightVAO);
    glDeleteBuffers(1, &m_VBO);
}

Status LGL_2::event(SDL_Event &event)
{
    switch (event.type) {
        case SDL_KEYUP:
            if (SDL_SCANCODE_RETURN == event.key.keysym.scancode)
                m_enableSpotLight = !m_enableSpotLight;
            break;
        default:
            break;
    }
    return Status::Ok;
}

Status LGL_2::render()
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_diffuseMap.activate(0);
    m_specularMap.activate(1);

    m_lightingShader.use();

    m_lightingShader.set_mat4("view", m_view);
    m_lightingShader.set_mat4("projection", m_projection);

    m_lightingShader.set_vec3("viewPos", m_camera.position);
    m_lightingShader.set_float("material.shininess", 64.0f);

    // Point lights use range of 50
    // https://wiki.ogre3d.org/tiki-index.php?page=-Point+Light+Attenuation
    m_lightingShader.set_vec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
    m_lightingShader.set_vec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
    m_lightingShader.set_vec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
    m_lightingShader.set_vec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

    m_lightingShader.set_vec3("pointLights[0].position", pointLightPositions[0]);
    m_lightingShader.set_float("pointLights[0].constant", 1.0f);
    m_lightingShader.set_float("pointLights[0].linear", 0.09f);
    m_lightingShader.set_float("pointLights[0].quadratic", 0.032f);
    m_lightingShader.set_vec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
    m_lightingShader.set_vec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
    m_lightingShader.set_vec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);

    m_lightingShader.set_vec3("pointLights[1].position", pointLightPositions[1]);
    m_lightingShader.set_float("pointLights[1].constant", 1.0f);
    m_lightingShader.set_float("pointLights[1].linear", 0.09f);
    m_lightingShader.set_float("pointLights[1].quadratic", 0.032f);
    m_lightingShader.set_vec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
    m_lightingShader.set_vec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
    m_lightingShader.set_vec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);

    m_lightingShader.set_vec3("pointLights[2].position", pointLightPositions[2]);
    m_lightingShader.set_float("pointLights[2].constant", 1.0f);
    m_lightingShader.set_float("pointLights[2].linear", 0.09f);
    m_lightingShader.set_float("pointLights[2].quadratic", 0.032f);
    m_lightingShader.set_vec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
    m_lightingShader.set_vec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
    m_lightingShader.set_vec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);

    m_lightingShader.set_vec3("pointLights[3].position", pointLightPositions[3]);
    m_lightingShader.set_float("pointLights[3].constant", 1.0f);
    m_lightingShader.set_float("pointLights[3].linear", 0.09f);
    m_lightingShader.set_float("pointLights[3].quadratic", 0.032f);
    m_lightingShader.set_vec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
    m_lightingShader.set_vec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
    m_lightingShader.set_vec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);

    m_lightingShader.set_vec3("spotLight.position", m_camera.position);
    m_lightingShader.set_vec3("spotLight.direction", m_camera.front);
    m_lightingShader.set_float("spotLight.cutoff", glm::cos(glm::radians(12.5f)));
    m_lightingShader.set_float("spotLight.outercutoff", glm::cos(glm::radians(15.0f)));
    m_lightingShader.set_float("spotLight.constant", 1.0f);
    m_lightingShader.set_float("spotLight.linear", 0.09f);
    m_lightingShader.set_float("spotLight.quadratic", 0.032f);
    if (m_enableSpotLight) {
        m_lightingShader.set_vec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        m_lightingShader.set_vec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        m_lightingShader.set_vec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
    } else {
        m_lightingShader.set_vec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        m_lightingShader.set_vec3("spotLight.diffuse", 0.0f, 0.0f, 0.0f);
        m_lightingShader.set_vec3("spotLight.specular", 0.0f, 0.0f, 0.0f);
    }

    glm::mat4 model = glm::mat4(1.0f);

    glBindVertexArray(m_VAO);
    int ncubes = sizeof(cubePositions) / sizeof(glm::vec3);
    for (int i = 0; i < ncubes; ++i) {
        model = glm::mat4(1.0f);
        model = glm::translate(model, cubePositions[i]);
        float angle = 20.0f * i;
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        m_lightingShader.set_mat4("model", model);

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    m_lightShader.use();

    m_lightShader.set_mat4("view", m_view);
    m_lightShader.set_mat4("projection", m_projection);

    glBindVertexArray(m_lightVAO);
    int nplights = sizeof(pointLightPositions) / sizeof(glm::vec3);
    for (int i = 0; i < nplights; ++i) {
        model = glm::mat4(1.0f);
        model = glm::translate(model, pointLightPositions[i]);
        model = glm::scale(model, glm::vec3(0.2f));
        m_lightShader.set_mat4("model", model);

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    SDL_GL_SwapWindow(m_window);

    return Status::Ok;
}

GLLELU_MAIN_IMPLEMENTATION(LGL_2)
