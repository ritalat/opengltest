#include "gllelucamera.hh"
#include "gllelu_main.hh"
#include "shader.hh"
#include "shapes.hh"
#include "texture.hh"

#include "glad/gl.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "SDL3/SDL.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

struct Vert {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
    glm::vec3 tangent;
};

const glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

class NormalMap: public GLleluCamera
{
public:
    NormalMap(int argc, char *argv[]);
    virtual ~NormalMap();

protected:
    virtual Status render();

private:
    Shader m_lightingShader;
    Texture m_diffuseMap;
    Texture m_normalMap;
    unsigned int m_VAO;
    unsigned int m_VBO;
};

NormalMap::NormalMap(int argc, char *argv[]):
    GLleluCamera(argc, argv),
    m_lightingShader("normalmap.vert", "normalmap.frag"),
    m_diffuseMap("lgl_brickwall.jpg"),
    m_normalMap("lgl_brickwall_normal.png"),
    m_VAO(0),
    m_VBO(0)
{
    m_lightingShader.use();
    m_lightingShader.setInt("material.diffuse", 0);
    m_lightingShader.setInt("material.normal", 1);

    unsigned int numTris = sizeof(cube) / sizeof(float) / 8 / 3;
    std::vector<Vert> cubeVerts(numTris * 3);

    for (unsigned int i = 0; i < numTris; ++i) {
        unsigned int j = i * 3 * 8;

        Vert vert[3];
        memcpy(&vert[0], &cube[j], 8 * sizeof(float));
        memcpy(&vert[1], &cube[j + 8], 8 * sizeof(float));
        memcpy(&vert[2], &cube[j+ 16], 8 * sizeof(float));

        glm::vec3 edge1 = vert[1].position - vert[0].position;
        glm::vec3 edge2 = vert[2].position - vert[0].position;
        glm::vec2 deltaUV1 = vert[1].uv - vert[0].uv;
        glm::vec2 deltaUV2 = vert[2].uv - vert[0].uv;

        glm::vec3 tangent;
        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
        tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

        vert[0].tangent = vert[1].tangent = vert[2].tangent = tangent;
        memcpy(&cubeVerts[i * 3], &vert[0], sizeof(vert));
    }

    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, cubeVerts.size() * sizeof(Vert), cubeVerts.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

NormalMap::~NormalMap()
{
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
}

Status NormalMap::render()
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_diffuseMap.activate(0);
    m_normalMap.activate(1);

    m_lightingShader.use();

    m_lightingShader.setMat4("view", view());
    m_lightingShader.setMat4("projection", projection());

    m_lightingShader.setFloat("material.shininess", 64.0f);
    m_lightingShader.setVec3("lightPos", lightPos);
    m_lightingShader.setVec3("viewPos", camera().position);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(static_cast<float>(SDL_GetTicks()) / 50.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    m_lightingShader.setMat4("model", model);

    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    swapWindow();

    return Status::Ok;
}

GLLELU_MAIN_IMPLEMENTATION(NormalMap)
