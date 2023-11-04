#include "gllelucamera.hh"
#include "shader.hh"

#include "glad/gl.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "SDL.h"

#include <cstdio>
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

const int numCubes = 10;

const glm::vec3 cubePositions[numCubes] = {
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

struct BasicMaterial {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
};

// http://devernay.free.fr/cours/opengl/materials.html
const BasicMaterial cubeMaterials[numCubes] = {
    {   // Emerald
        glm::vec3( 0.0215f,  0.1745f,  0.0215f),
        glm::vec3( 0.07568f,  0.61424f,  0.07568f),
        glm::vec3( 0.633f,  0.727811f,  0.633f),
        128 * 0.6
    },
    {   // Pearl
        glm::vec3( 0.25f,  0.20725f,  0.20725f),
        glm::vec3( 1.0f,  0.829f,  0.829f),
        glm::vec3( 0.296648f,  0.296648f,  0.296648f),
        128 * 0.088
    },
    {   // Bronze
        glm::vec3( 0.2125f,  0.1275f,  0.054f),
        glm::vec3( 0.714f,  0.4284f,  0.18144f),
        glm::vec3( 0.393548f,  0.271906f,  0.166721f),
        128 * 0.2
    },
    {   // Gold
        glm::vec3( 0.24725f,  0.1995f,  0.0745f),
        glm::vec3( 0.75164f,  0.60648f,  0.22648f),
        glm::vec3( 0.628281f,  0.555802f,  0.366065f),
        128 * 0.4
    },
    {   // Cyan Plastic
        glm::vec3( 0.0f,  0.1f,  0.06f),
        glm::vec3( 0.0f,  0.50980392f,  0.50980392f),
        glm::vec3( 0.50196078f,  0.50196078f,  0.50196078f),
        128 * 0.25
    },
    {   // Red Plastic
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 0.5f,  0.0f,  0.0f),
        glm::vec3( 0.7f,  0.6f,  0.6f),
        128 * 0.25
    },
    {   // Green Rubber
        glm::vec3( 0.0f,  0.05f,  0.0f),
        glm::vec3( 0.4f,  0.5f,  0.4f),
        glm::vec3( 0.04f,  0.7f,  0.04f),
        128 * 0.078125
    },
    {   // Yellow Rubber
        glm::vec3( 0.05f,  0.05f,  0.0f),
        glm::vec3( 0.5f,  0.5f,  0.4f),
        glm::vec3( 0.7f,  0.7f,  0.04f),
        128 * 0.078125
    },
    {   // Silver
        glm::vec3( 0.19225f,  0.19225f,  0.19225f),
        glm::vec3( 0.50754f,  0.50754f,  0.50754f),
        glm::vec3( 0.508273f,  0.508273f,  0.508273f),
        128 * 0.4
    },
    {   // Ruby
        glm::vec3( 0.1745f,  0.01175f,  0.01175f),
        glm::vec3( 0.61424f,  0.04136f,  0.04136f),
        glm::vec3( 0.727811f,  0.626959f,  0.626959f),
        128 * 0.6
    }
};

const glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

class LGL_2_Basic: public GLleluCamera
{
public:
    LGL_2_Basic(int argc, char *argv[]);
    virtual ~LGL_2_Basic();
    virtual void render();

    Shader lightingShader;
    Shader lightShader;
    unsigned int VAO;
    unsigned int lightVAO;
    unsigned int VBO;
};

LGL_2_Basic::LGL_2_Basic(int argc, char *argv[]):
    GLleluCamera(argc, argv)
{
    glEnable(GL_DEPTH_TEST);

    if (!lightingShader.load("lighting.vert", "lighting_basic.frag")) {
        quit = true;
        status = EXIT_FAILURE;
        return;
    }

    if (!lightShader.load("lighting.vert", "lighting_light.frag")) {
        quit = true;
        status = EXIT_FAILURE;
        return;
    }

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

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

LGL_2_Basic::~LGL_2_Basic()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);
}

void LGL_2_Basic::render()
{
    glm::mat4 projection = glm::perspective(glm::radians(camera.fov), (float)fbSize.width / (float)fbSize.height, 0.1f, 100.0f);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    lightingShader.use();

    lightingShader.set_mat4("projection", projection);
    lightingShader.set_mat4("view", view);

    lightingShader.set_vec3("light.ambient", 1.0f, 1.0f, 1.0f);
    lightingShader.set_vec3("light.diffuse", 1.0f, 1.0f, 1.0f);
    lightingShader.set_vec3("light.specular", 1.0f, 1.0f, 1.0f);
    lightingShader.set_vec3("light.position", lightPos);

    lightingShader.set_vec3("viewPos", camera.position);

    glm::mat4 model = glm::mat4(1.0f);

    glBindVertexArray(VAO);
    for (int i = 0; i < numCubes; ++i) {
        model = glm::mat4(1.0f);
        model = glm::translate(model, cubePositions[i]);
        float angle = 20.0f * i;
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        lightingShader.set_mat4("model", model);

        lightingShader.set_vec3("material.ambient", cubeMaterials[i].ambient);
        lightingShader.set_vec3("material.diffuse", cubeMaterials[i].diffuse);
        lightingShader.set_vec3("material.specular", cubeMaterials[i].specular);
        lightingShader.set_float("material.shininess", cubeMaterials[i].shininess);

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

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
    LGL_2_Basic lgl_2_basic(argc, argv);
    return lgl_2_basic.run();
}
