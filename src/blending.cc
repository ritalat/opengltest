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

#include <map>

const glm::vec3 planes[] = {
    glm::vec3(-1.0f, 0.0f, -0.8f),
    glm::vec3(1.5f, 0.0f, 0.5f),
    glm::vec3(0.0f, 0.0f, 0.7f),
    glm::vec3(-0.3f, 0.0f, -2.3f),
    glm::vec3(0.5f, 0.0f, -0.49f)
};

const int numPlanes = sizeof(planes) / sizeof(glm::vec3);

class Blending: public GLlelu
{
public:
    Blending(int argc, char *argv[]);
    virtual ~Blending();
    virtual SDL_AppResult event(SDL_Event *event);
    virtual SDL_AppResult iterate();

private:
    Camera m_camera;
    Shader m_alphaShader;
    Shader m_blendingShader;
    Texture m_floorTexture;
    Texture m_cubeTexture;
    Texture m_grassTexture;
    Texture m_windowTexture;
    bool m_simpleAlphaDiscard;
    unsigned int m_planeVAO;
    unsigned int m_planeVBO;
    unsigned int m_cubeVAO;
    unsigned int m_cubeVBO;
};

Blending::Blending(int argc, char *argv[]):
    GLlelu(argc, argv),
    m_camera(this),
    m_alphaShader("blending.vert", "simple_alpha.frag"),
    m_blendingShader("blending.vert", "blending.frag"),
    m_floorTexture("lgl_metal.png"),
    m_cubeTexture("lgl_marble.jpg"),
    m_grassTexture("lgl_grass.png"),
    m_windowTexture("lgl_window.png"),
    m_simpleAlphaDiscard(true),
    m_planeVAO(0),
    m_planeVBO(0),
    m_cubeVAO(0),
    m_cubeVBO(0)
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_alphaShader.use();
    m_alphaShader.setInt("texture0", 0);
    m_blendingShader.use();
    m_blendingShader.setInt("texture0", 0);

    m_grassTexture.wrapping(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    m_windowTexture.wrapping(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

    glGenVertexArrays(1, &m_planeVAO);
    glBindVertexArray(m_planeVAO);

    glGenBuffers(1, &m_planeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(plane), plane, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glGenVertexArrays(1, &m_cubeVAO);
    glBindVertexArray(m_cubeVAO);

    glGenBuffers(1, &m_cubeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Blending::~Blending()
{
    glDeleteVertexArrays(1, &m_planeVAO);
    glDeleteBuffers(1, &m_planeVBO);
    glDeleteVertexArrays(1, &m_cubeVAO);
    glDeleteBuffers(1, &m_cubeVBO);
}

SDL_AppResult Blending::event(SDL_Event *event)
{
    switch (event->type) {
        case SDL_EVENT_KEY_UP:
            if (SDL_SCANCODE_RETURN == event->key.scancode)
                m_simpleAlphaDiscard = !m_simpleAlphaDiscard;
            break;
        default:
            break;
    }

    m_camera.event(event);
    return GLlelu::event(event);
}

SDL_AppResult Blending::iterate()
{
    m_camera.iterate();

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_blendingShader.use();
    m_blendingShader.setMat4("view", m_camera.view());
    m_blendingShader.setMat4("projection", m_camera.projection());

    m_floorTexture.activate(0);

    glBindVertexArray(m_planeVAO);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -0.51f, 0.0f));
    model = glm::scale(model, glm::vec3(5.0f, 1.0f, 5.0f));
    m_blendingShader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    m_cubeTexture.activate(0);

    glBindVertexArray(m_cubeVAO);
    model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.0f, 1.0f));
    m_blendingShader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    model = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, -1.0f));
    m_blendingShader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glDisable(GL_CULL_FACE);
    if (m_simpleAlphaDiscard) {
        m_alphaShader.use();
        m_alphaShader.setMat4("view", m_camera.view());
        m_alphaShader.setMat4("projection", m_camera.projection());
        m_grassTexture.activate(0);

        glBindVertexArray(m_planeVAO);

        float angle = -40.0f;
        for (int i = 0; i < numPlanes; ++i) {
            model = glm::translate(glm::mat4(1.0f), planes[i]);
            model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
            model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
            m_alphaShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            angle += 20.0f;
        }
    } else {
        m_windowTexture.activate(0);

        glBindVertexArray(m_planeVAO);

        std::map<float, glm::vec3> sortedPlanes;
        for (int i = 0; i < numPlanes; ++i) {
            float distance = glm::length(m_camera.data.position - planes[i]);
            sortedPlanes[distance] = planes[i];
        }

        for (auto it = sortedPlanes.rbegin(); it != sortedPlanes.rend(); ++it) {
            auto [dist, pos] = *it;
            model = glm::translate(glm::mat4(1.0f), pos);
            model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
            m_blendingShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
    }
    glEnable(GL_CULL_FACE);

    return GLlelu::iterate();
}

GLLELU_MAIN_IMPLEMENTATION(Blending)
