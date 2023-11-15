#include "gllelucamera.hh"
#include "model.hh"
#include "shader.hh"
#include "texture.hh"

#include "glad/gl.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "SDL.h"

#include <cstdlib>

const float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f, 0.0f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f, 0.0f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f, 0.0f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f
};

const glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

class ObjFiles: public GLleluCamera
{
public:
    ObjFiles(int argc, char *argv[]);
    virtual ~ObjFiles();
    virtual void render();

    Shader lightingShader;
    Shader lightShader;
    Model objModel;
    unsigned int objTexture;
    unsigned int lightVAO;
    unsigned int lightVBO;
};

ObjFiles::ObjFiles(int argc, char *argv[]):
    GLleluCamera(argc, argv)
{
    glEnable(GL_DEPTH_TEST);

    if (!lightingShader.load("lighting.vert", "lighting_textured.frag")) {
        quit = true;
        status = EXIT_FAILURE;
        return;
    }

    if (!lightShader.load("lighting.vert", "lighting_light.frag")) {
        quit = true;
        status = EXIT_FAILURE;
        return;
    }

    if (!load_texture(objTexture, "vt_viking_room.png")) {
        quit = true;
        status = EXIT_FAILURE;
        return;
    }

    lightingShader.use();
    lightingShader.set_int("material.diffuse", 0);
    lightingShader.set_int("material.specular", 0);

    if (!load_obj(objModel, "vt_viking_room.obj")) {
        quit = true;
        status = EXIT_FAILURE;
        return;
    }

    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);

    glGenBuffers(1, &lightVBO);
    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

ObjFiles::~ObjFiles()
{
    glDeleteTextures(1, &objTexture);
    glDeleteVertexArrays(1, &objModel.VAO);
    glDeleteBuffers(1, &objModel.VBO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &lightVBO);
}

void ObjFiles::render()
{
    glm::mat4 projection = glm::perspective(glm::radians(camera.fov), (float)fbSize.width / (float)fbSize.height, 0.1f, 100.0f);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, objTexture);

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
    model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    lightingShader.set_mat4("model", model);

    glBindVertexArray(objModel.VAO);
    glDrawArrays(GL_TRIANGLES, 0, objModel.vertices.size());

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
}

int main(int argc, char *argv[])
{
    ObjFiles objFiles(argc, argv);
    return objFiles.run();
}
