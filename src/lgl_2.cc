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

struct DirLight {
    glm::vec4 direction;

    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;
};

const int MAX_POINT_LIGHTS = 10;

struct PointLight {
    glm::vec4 position;

    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;

    float constant;
    float linear;
    float quadratic;

    float padding;
};

struct SpotLight {
    glm::vec4 position;
    glm::vec4 direction;

    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;

    float constant;
    float linear;
    float quadratic;

    float cutoff;
    float outercutoff;

    float padding[3];
};

struct LightUniformBuffer {
    SpotLight spotLight;
    DirLight dirLight;
    PointLight pointLights[MAX_POINT_LIGHTS];
};

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

const int numPointLights = sizeof(pointLightPositions) / sizeof(glm::vec3);

class LGL2: public GLleluCamera
{
public:
    LGL2(int argc, char *argv[]);
    virtual ~LGL2();

protected:
    virtual Status event(SDL_Event &event);
    virtual Status render();

private:
    Shader m_lightingShader;
    Shader m_lightShader;
    Texture m_diffuseMap;
    Texture m_specularMap;
    unsigned int m_VAO;
    unsigned int m_lightVAO;
    unsigned int m_VBO;
    unsigned int m_UBO;
    bool m_enableSpotLight;
};

LGL2::LGL2(int argc, char *argv[]):
    GLleluCamera(argc, argv),
    m_lightingShader("lighting.vert", "lighting.frag"),
    m_lightShader("light.vert", "light.frag"),
    m_diffuseMap("lgl_container2.png"),
    m_specularMap("lgl_container2_specular.png"),
    m_VAO(0),
    m_lightVAO(0),
    m_VBO(0),
    m_UBO(0),
    m_enableSpotLight(true)
{
    m_lightingShader.use();
    m_lightingShader.setInt("material.diffuse", 0);
    m_lightingShader.setInt("material.specular", 1);
    m_lightingShader.setFloat("material.shininess", 64.0f);

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

    glGenBuffers(1, &m_UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, m_UBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(LightUniformBuffer), NULL, GL_DYNAMIC_DRAW);

    // Point lights use range of 50
    // https://wiki.ogre3d.org/-Point+Light+Attenuation
    LightUniformBuffer uboData {};

    uboData.spotLight.position = glm::vec4(camera().position, 0.0f);
    uboData.spotLight.direction = glm::vec4(camera().front, 0.0f);
    uboData.spotLight.ambient = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    uboData.spotLight.diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);
    uboData.spotLight.specular = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);
    uboData.spotLight.constant = 1.0f;
    uboData.spotLight.linear = 0.09f;
    uboData.spotLight.quadratic = 0.032f;
    uboData.spotLight.cutoff = glm::cos(glm::radians(12.5f));
    uboData.spotLight.outercutoff = glm::cos(glm::radians(15.0f));

    uboData.dirLight.direction = glm::vec4(-0.2f, -1.0f, -0.3f, 0.0f);
    uboData.dirLight.ambient = glm::vec4(0.05f, 0.05f, 0.05f, 0.0f);
    uboData.dirLight.diffuse = glm::vec4(0.4f, 0.4f, 0.4f, 0.0f);
    uboData.dirLight.specular = glm::vec4(0.5f, 0.5f, 0.5f, 0.0f);

    int lights = numPointLights < MAX_POINT_LIGHTS ? numPointLights : MAX_POINT_LIGHTS;

    for (int i = 0; i < lights; ++i) {
        uboData.pointLights[i].position = glm::vec4(pointLightPositions[i], 0.0f);
        uboData.pointLights[i].ambient = glm::vec4(0.05f, 0.05f, 0.05f, 0.0f);
        uboData.pointLights[i].diffuse = glm::vec4(0.8f, 0.8f, 0.8f, 0.0f);
        uboData.pointLights[i].specular = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);
        uboData.pointLights[i].constant = 1.0f;
        uboData.pointLights[i].linear = 0.09f;
        uboData.pointLights[i].quadratic = 0.032f;
    }

    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(LightUniformBuffer), &uboData);

    unsigned int lightsIndex = glGetUniformBlockIndex(m_lightingShader.id(), "Lights");
    glUniformBlockBinding(m_lightingShader.id(), lightsIndex, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_UBO);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    m_lightingShader.setInt("numPointLights", lights);
}

LGL2::~LGL2()
{
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteVertexArrays(1, &m_lightVAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_UBO);
}

Status LGL2::event(SDL_Event &event)
{
    switch (event.type) {
        case SDL_EVENT_KEY_UP:
            if (SDL_SCANCODE_RETURN == event.key.scancode)
                m_enableSpotLight = !m_enableSpotLight;
            break;
        default:
            break;
    }
    return Status::Ok;
}

Status LGL2::render()
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_diffuseMap.activate(0);
    m_specularMap.activate(1);

    m_lightingShader.use();

    m_lightingShader.setMat4("view", view());
    m_lightingShader.setMat4("projection", projection());

    m_lightingShader.setVec3("viewPos", camera().position);
    m_lightingShader.setBool("enableSpotLight", m_enableSpotLight);

    glBindBuffer(GL_UNIFORM_BUFFER, m_UBO);
    glm::vec4 spotLightPos[2] = { glm::vec4(camera().position, 0.0f), glm::vec4(camera().front, 0.0f) };
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(spotLightPos), &spotLightPos);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glm::mat4 model = glm::mat4(1.0f);

    glBindVertexArray(m_VAO);
    int ncubes = sizeof(cubePositions) / sizeof(glm::vec3);
    for (int i = 0; i < ncubes; ++i) {
        model = glm::mat4(1.0f);
        model = glm::translate(model, cubePositions[i]);
        float angle = 20.0f * static_cast<float>(i);
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        m_lightingShader.setMat4("model", model);
        m_lightingShader.setMat4("normalMat", glm::transpose(glm::inverse(model)));

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    m_lightShader.use();

    m_lightShader.setMat4("view", view());
    m_lightShader.setMat4("projection", projection());

    glBindVertexArray(m_lightVAO);
    int nplights = sizeof(pointLightPositions) / sizeof(glm::vec3);
    for (int i = 0; i < nplights; ++i) {
        model = glm::mat4(1.0f);
        model = glm::translate(model, pointLightPositions[i]);
        model = glm::scale(model, glm::vec3(0.2f));
        m_lightShader.setMat4("model", model);

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    swapWindow();

    return Status::Ok;
}

GLLELU_MAIN_IMPLEMENTATION(LGL2)
