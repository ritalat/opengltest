#include "gllelucamera.hh"
#include "gllelu_main.hh"
#include "shader.hh"
#include "shapes.hh"
#include "texture.hh"

#include "glad/gl.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "SDL.h"

#include <cstdlib>
#include <stdexcept>

struct Light {
    glm::vec4 position;
    glm::vec4 color;
};

#define NUM_LIGHTS 20

enum class Visualization {
    NONE,
    POSITION,
    NORMAL,
    COLOR,
    SPECULAR,
    END
};

const glm::vec3 cubePositions[] = {
    glm::vec3( 1.5f, -0.25f,  1.5f),
    glm::vec3( 1.5f, -0.25f,  0.5f),
    glm::vec3( 1.5f, -0.25f, -0.5f),
    glm::vec3( 1.5f, -0.25f, -1.5f),
    glm::vec3( 0.5f, -0.25f,  1.5f),
    glm::vec3( 0.5f, -0.25f,  0.5f),
    glm::vec3( 0.5f, -0.25f, -0.5f),
    glm::vec3( 0.5f, -0.25f, -1.5f),
    glm::vec3(-0.5f, -0.25f,  1.5f),
    glm::vec3(-0.5f, -0.25f,  0.5f),
    glm::vec3(-0.5f, -0.25f, -0.5f),
    glm::vec3(-0.5f, -0.25f, -1.5f),
    glm::vec3(-1.5f, -0.25f,  1.5f),
    glm::vec3(-1.5f, -0.25f,  0.5f),
    glm::vec3(-1.5f, -0.25f, -0.5f),
    glm::vec3(-1.5f, -0.25f, -1.5f),
};

const float floorPlane[] = {
    -1.0f, 0.0f, -1.0f,   0.0f, 1.0f, 0.0f,   0.0f, 2.5f,
     1.0f, 0.0f,  1.0f,   0.0f, 1.0f, 0.0f,   2.5f, 0.0f,
     1.0f, 0.0f, -1.0f,   0.0f, 1.0f, 0.0f,   2.5f, 2.5f,
     1.0f, 0.0f,  1.0f,   0.0f, 1.0f, 0.0f,   2.5f, 0.0f,
    -1.0f, 0.0f, -1.0f,   0.0f, 1.0f, 0.0f,   0.0f, 2.5f,
    -1.0f, 0.0f,  1.0f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f
};

class Deferred: public GLleluCamera
{
public:
    Deferred(int argc, char *argv[]);
    virtual ~Deferred();

protected:
    virtual Status event(SDL_Event &event);
    virtual Status render();
    void recreate_gbuffer();

private:
    Shader m_deferredLighting;
    Shader m_geometryPass;
    Texture m_floorDiffuse;
    Texture m_floorSpecular;
    Texture m_cubeDiffuse;
    Texture m_cubeSpecular;
    unsigned int m_planeVAO;
    unsigned int m_planeVBO;
    unsigned int m_cubeVAO;
    unsigned int m_cubeVBO;
    unsigned int m_dummyVAO;
    unsigned int m_gbuffer;
    unsigned int m_gposition;
    unsigned int m_gnormal;
    unsigned int m_gcolor;
    unsigned int m_gdepth;
    unsigned int m_lightUBO;
    Light m_lights[NUM_LIGHTS];
    Visualization m_visualizedBuffer;
};

Deferred::Deferred(int argc, char *argv[]):
    GLleluCamera(argc, argv),
    m_deferredLighting("fullscreen.vert", "deferred.frag"),
    m_geometryPass("deferred_gbuffer.vert", "deferred_gbuffer.frag"),
    m_floorDiffuse("lgl_wall.jpg"),
    m_floorSpecular("none_specular.png"),
    m_cubeDiffuse("lgl_container2.png"),
    m_cubeSpecular("lgl_container2_specular.png"),
    m_planeVAO(0),
    m_planeVBO(0),
    m_cubeVAO(0),
    m_cubeVBO(0),
    m_dummyVAO(0),
    m_gbuffer(0),
    m_gposition(0),
    m_gnormal(0),
    m_gcolor(0),
    m_gdepth(0),
    m_lightUBO(0),
    m_visualizedBuffer(Visualization::NONE)
{
    m_deferredLighting.use();
    m_deferredLighting.set_int("gposition", 0);
    m_deferredLighting.set_int("gnormal", 1);
    m_deferredLighting.set_int("gcolor", 2);
    m_geometryPass.use();
    m_geometryPass.set_int("diffuse", 0);
    m_geometryPass.set_int("specular", 1);

    camera().position = glm::vec3(2.5f, 1.5f, 3.5f);
    camera().pitch = -20.0f;
    camera().yaw = -127.5f;

    glGenBuffers(1, &m_lightUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, m_lightUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(m_lights), NULL, GL_DYNAMIC_DRAW);

    for (int i = 0; i < NUM_LIGHTS; ++i) {
        m_lights[i].position = glm::vec4(
            static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 5.0f - 2.5f,
            static_cast<float>(rand()) / static_cast<float>(RAND_MAX),
            static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 5.0f - 2.5f,
            0.0f
        );
        m_lights[i].color = glm::vec4(
            static_cast<float>(rand()) / static_cast<float>(RAND_MAX) / 2.0f,
            static_cast<float>(rand()) / static_cast<float>(RAND_MAX) / 2.0f,
            static_cast<float>(rand()) / static_cast<float>(RAND_MAX) / 2.0f,
            0.0f
        );
        int primaryColor = rand() % 3;
        if (primaryColor == 0) {
            m_lights[i].color.r = 1.0f;
        } else if (primaryColor == 1) {
            m_lights[i].color.g = 1.0f;
        } else {
            m_lights[i].color.b = 1.0f;
        }
    }

    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(m_lights), &m_lights);
    unsigned int lightsIndex = glGetUniformBlockIndex(m_deferredLighting.id(), "Lights");
    glUniformBlockBinding(m_deferredLighting.id(), lightsIndex, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_lightUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    m_deferredLighting.set_int("numLights", NUM_LIGHTS);

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

    glGenVertexArrays(1, &m_dummyVAO);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    recreate_gbuffer();
}

Deferred::~Deferred()
{
    glDeleteVertexArrays(1, &m_planeVAO);
    glDeleteBuffers(1, &m_planeVBO);
    glDeleteVertexArrays(1, &m_cubeVAO);
    glDeleteBuffers(1, &m_cubeVBO);
    glDeleteVertexArrays(1, &m_dummyVAO);
    glDeleteFramebuffers(1, &m_gbuffer);
    glDeleteTextures(1, &m_gposition);
    glDeleteTextures(1, &m_gnormal);
    glDeleteTextures(1, &m_gcolor);
    glDeleteRenderbuffers(1, &m_gdepth);
    glDeleteBuffers(1, &m_lightUBO);
}

Status Deferred::event(SDL_Event &event)
{
    switch (event.type) {
        case SDL_KEYUP:
            if (SDL_SCANCODE_RETURN == event.key.keysym.scancode) {
                int tmp = static_cast<int>(m_visualizedBuffer);
                ++tmp;
                if (tmp >= static_cast<int>(Visualization::END))
                    tmp = 0;
                m_visualizedBuffer = static_cast<Visualization>(tmp);
            }
            break;
        case SDL_WINDOWEVENT:
            if (event.window.windowID == window_id()) {
                switch (event.window.event) {
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                        recreate_gbuffer();
                        break;
                    default:
                        break;
                }
            }
            break;
        default:
            break;
    }
    return Status::Ok;
}

Status Deferred::render()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_gbuffer);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_geometryPass.use();
    m_geometryPass.set_mat4("view", view());
    m_geometryPass.set_mat4("projection", projection());

    m_floorDiffuse.activate(0);
    m_floorSpecular.activate(1);

    glBindVertexArray(m_planeVAO);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -0.51f, 0.0f));
    model = glm::scale(model, glm::vec3(2.5f, 1.0f, 2.5f));
    m_geometryPass.set_mat4("model", model);
    m_geometryPass.set_mat4("normalMat", glm::transpose(glm::inverse(model)));
    glDrawArrays(GL_TRIANGLES, 0, 6);

    m_cubeDiffuse.activate(0);
    m_cubeSpecular.activate(1);

    glBindVertexArray(m_cubeVAO);
    for (size_t i = 0; i < sizeof(cubePositions) / sizeof(glm::vec3); ++i) {
        model = glm::translate(glm::mat4(1.0f), cubePositions[i]);
        model = glm::scale(model, glm::vec3(0.5f));
        m_geometryPass.set_mat4("model", model);
        m_geometryPass.set_mat4("normalMat", glm::transpose(glm::inverse(model)));
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    m_deferredLighting.use();
    m_deferredLighting.set_int("numLights", NUM_LIGHTS);
    m_deferredLighting.set_vec3("viewPos", camera().position);
    m_deferredLighting.set_int("visualizedBuffer", static_cast<int>(m_visualizedBuffer));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_gposition);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_gnormal);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_gcolor);
    glBindVertexArray(m_dummyVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    swap_window();

    return Status::Ok;
}

void Deferred::recreate_gbuffer()
{
    if (m_gposition)
        glDeleteTextures(1, &m_gposition);
    if (m_gnormal)
        glDeleteTextures(1, &m_gnormal);
    if (m_gcolor)
        glDeleteTextures(1, &m_gcolor);
    if (m_gdepth)
        glDeleteRenderbuffers(1, &m_gdepth);

    if (!m_gbuffer)
        glGenFramebuffers(1, &m_gbuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_gbuffer);

    int width = fb_size().width;
    int height = fb_size().height;

    glGenTextures(1, &m_gposition);
    glBindTexture(GL_TEXTURE_2D, m_gposition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glGenTextures(1, &m_gnormal);
    glBindTexture(GL_TEXTURE_2D, m_gnormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glGenTextures(1, &m_gcolor);
    glBindTexture(GL_TEXTURE_2D, m_gcolor);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, 0);

    glGenRenderbuffers(1, &m_gdepth);
    glBindRenderbuffer(GL_RENDERBUFFER, m_gdepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_gposition, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_gnormal, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_gcolor, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_gdepth);

    unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        throw std::runtime_error("Failed to create gbuffer");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLLELU_MAIN_IMPLEMENTATION(Deferred)
