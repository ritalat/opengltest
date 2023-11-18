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

const glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

class LGL_2_Textured: public GLleluCamera
{
public:
    LGL_2_Textured(int argc, char *argv[]);
    virtual ~LGL_2_Textured();
    virtual Status render();

    Shader lightingShader;
    Shader lightShader;
    Texture diffuseMap;
    Texture specularMap;
    unsigned int VAO;
    unsigned int lightVAO;
    unsigned int VBO;
};

LGL_2_Textured::LGL_2_Textured(int argc, char *argv[]):
    GLleluCamera(argc, argv)
{
    glEnable(GL_DEPTH_TEST);

    lightingShader.load("lighting.vert", "lighting_textured.frag");
    lightShader.load("lighting.vert", "lighting_light.frag");

    diffuseMap.load("lgl_container2.png");
    specularMap.load("lgl_container2_specular.png");

    lightingShader.use();
    lightingShader.set_int("material.diffuse", 0);
    lightingShader.set_int("material.specular", 1);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

LGL_2_Textured::~LGL_2_Textured()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);
}

Status LGL_2_Textured::render()
{
    glm::mat4 projection = glm::perspective(glm::radians(camera.fov), (float)fbSize.width / (float)fbSize.height, 0.1f, 100.0f);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    diffuseMap.activate(0);
    specularMap.activate(1);

    lightingShader.use();

    lightingShader.set_mat4("projection", projection);
    lightingShader.set_mat4("view", view);

    lightingShader.set_float("material.shininess", 64.0f);
    lightingShader.set_vec3("light.ambient", 0.2f, 0.2f, 0.2f);
    lightingShader.set_vec3("light.diffuse", 0.5f, 0.5f, 0.5f);
    lightingShader.set_vec3("light.specular", 1.0f, 1.0f, 1.0f);
    lightingShader.set_vec3("light.position", lightPos);
    lightingShader.set_vec3("viewPos", camera.position);

    glm::mat4 model = glm::mat4(1.0f);
    lightingShader.set_mat4("model", model);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    lightShader.use();

    lightShader.set_mat4("projection", projection);
    lightShader.set_mat4("view", view);

    model = glm::mat4(1.0f);
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.2f));
    lightShader.set_mat4("model", model);

    glBindVertexArray(lightVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    SDL_GL_SwapWindow(window);

    return Status::Ok;
}

GLLELU_MAIN_IMPLEMENTATION(LGL_2_Textured)
