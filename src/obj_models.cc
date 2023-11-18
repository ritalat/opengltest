#include "gllelucamera.hh"
#include "gllelu_main.hh"
#include "himmeli.hh"
#include "shader.hh"
#include "shapes.hh"

#include "glad/gl.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "SDL.h"

const glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

const BasicMaterial emerald {
    glm::vec3( 0.0215f,  0.1745f,  0.0215f),
    glm::vec3( 0.07568f,  0.61424f,  0.07568f),
    glm::vec3( 0.633f,  0.727811f,  0.633f),
    128 * 0.6
};

class ObjFiles: public GLleluCamera
{
public:
    ObjFiles(int argc, char *argv[]);
    virtual ~ObjFiles();
    virtual Status render();

    Shader lightingShader;
    Shader lightingShaderBasic;
    Shader lightShader;
    Himmeli room;
    Himmeli monkey;
    BasicHimmeli teapot;
    unsigned int lightVAO;
    unsigned int lightVBO;
};

ObjFiles::ObjFiles(int argc, char *argv[]):
    GLleluCamera(argc, argv)
{
    glEnable(GL_DEPTH_TEST);

    lightingShader.load("lighting.vert", "lighting_textured.frag");
    lightingShaderBasic.load("lighting.vert", "lighting_basic.frag");
    lightShader.load("lighting.vert", "lighting_light.frag");

    room.load("vt_viking_room.obj", "vt_viking_room.png");
    monkey.load("kultainenapina.obj", "kultainenapina.jpg");
    teapot.load("teapot.obj", emerald);

    room.rotate = glm::rotate(room.rotate, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    room.rotate = glm::rotate(room.rotate, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    room.translate = glm::translate(room.translate, glm::vec3(0.0f, -0.5f, 0.0f));

    monkey.scale = glm::scale(monkey.scale, glm::vec3(0.1f, 0.1f, 0.1f));
    monkey.rotate = glm::rotate(monkey.rotate, glm::radians(-45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    monkey.rotate = glm::rotate(monkey.rotate, glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    monkey.translate = glm::translate(monkey.translate, glm::vec3(0.475f, -0.3f, 0.0f));

    teapot.scale = glm::scale(teapot.scale, glm::vec3(0.1f, 0.1f, 0.1f));
    teapot.translate = glm::translate(teapot.translate, glm::vec3(0.0f, 0.5f, 0.0f));

    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);

    glGenBuffers(1, &lightVBO);
    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

ObjFiles::~ObjFiles()
{
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &lightVBO);
}

Status ObjFiles::render()
{
    glm::mat4 projection = glm::perspective(glm::radians(camera.fov), (float)fbSize.width / (float)fbSize.height, 0.1f, 100.0f);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    lightingShader.use();
    lightingShader.set_mat4("projection", projection);
    lightingShader.set_mat4("view", view);

    lightingShader.set_vec3("light.ambient", 0.2f, 0.2f, 0.2f);
    lightingShader.set_vec3("light.diffuse", 0.5f, 0.5f, 0.5f);
    lightingShader.set_vec3("light.specular", 1.0f, 1.0f, 1.0f);
    lightingShader.set_vec3("light.position", lightPos);
    lightingShader.set_vec3("viewPos", camera.position);

    room.draw(lightingShader);
    monkey.draw(lightingShader);

    lightingShaderBasic.use();
    lightingShaderBasic.set_mat4("projection", projection);
    lightingShaderBasic.set_mat4("view", view);

    lightingShaderBasic.set_vec3("light.ambient", 1.0f, 1.0f, 1.0f);
    lightingShaderBasic.set_vec3("light.diffuse", 1.0f, 1.0f, 1.0f);
    lightingShaderBasic.set_vec3("light.specular", 1.0f, 1.0f, 1.0f);
    lightingShaderBasic.set_vec3("light.position", lightPos);
    lightingShaderBasic.set_vec3("viewPos", camera.position);

    teapot.rotate = glm::rotate(glm::mat4(1.0f), glm::radians(SDL_GetTicks() / 10.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    teapot.draw(lightingShaderBasic);

    lightShader.use();
    lightShader.set_mat4("projection", projection);
    lightShader.set_mat4("view", view);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.2f));
    lightShader.set_mat4("model", model);

    glBindVertexArray(lightVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    SDL_GL_SwapWindow(window);

    return Status::Ok;
}

GLLELU_MAIN_IMPLEMENTATION(ObjFiles)
