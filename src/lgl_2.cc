#include "gllelucamera.hh"
#include "path.hh"
#include "shader.hh"
#include "texture.hh"

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
    virtual void event(SDL_Event &event);
    virtual void render();

    Shader lightingShader;
    Shader lightShader;
    Texture diffuseMap;
    Texture specularMap;
    unsigned int VAO;
    unsigned int lightVAO;
    unsigned int VBO;
    bool enableSpotLight;
};

LGL_2::LGL_2(int argc, char *argv[]):
    GLleluCamera(argc, argv),
    enableSpotLight(true)
{
    glEnable(GL_DEPTH_TEST);

    if (!lightingShader.load("lighting.vert", "lighting.frag")) {
        quit = true;
        status = EXIT_FAILURE;
        return;
    }

    if (!lightShader.load("lighting.vert", "lighting_light.frag")) {
        quit = true;
        status = EXIT_FAILURE;
        return;
    }

    if (!diffuseMap.load("lgl_container2.png")) {
        quit = true;
        status = EXIT_FAILURE;
        return;
    }

    if (!specularMap.load("lgl_container2_specular.png")) {
        quit = true;
        status = EXIT_FAILURE;
        return;
    }

    lightingShader.use();
    lightingShader.set_int("material.diffuse", 0);
    lightingShader.set_int("material.specular", 1);

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

LGL_2::~LGL_2()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);
}

void LGL_2::event(SDL_Event &event)
{
    switch (event.type) {
        case SDL_KEYUP:
            if (SDL_SCANCODE_RETURN == event.key.keysym.scancode)
                enableSpotLight = !enableSpotLight;
            break;
        default:
            break;
    }
}

void LGL_2::render()
{
    glm::mat4 projection = glm::perspective(glm::radians(camera.fov), (float)fbSize.width / (float)fbSize.height, 0.1f, 100.0f);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    diffuseMap.activate(0);
    specularMap.activate(1);

    lightingShader.use();

    lightingShader.set_mat4("projection", projection);
    lightingShader.set_mat4("view", view);

    lightingShader.set_vec3("viewPos", camera.position);
    lightingShader.set_float("material.shininess", 64.0f);

    // Point lights use range of 50
    // https://wiki.ogre3d.org/tiki-index.php?page=-Point+Light+Attenuation
    lightingShader.set_vec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
    lightingShader.set_vec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
    lightingShader.set_vec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
    lightingShader.set_vec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

    lightingShader.set_vec3("pointLights[0].position", pointLightPositions[0]);
    lightingShader.set_float("pointLights[0].constant", 1.0f);
    lightingShader.set_float("pointLights[0].linear", 0.09f);
    lightingShader.set_float("pointLights[0].quadratic", 0.032f);
    lightingShader.set_vec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
    lightingShader.set_vec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
    lightingShader.set_vec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);

    lightingShader.set_vec3("pointLights[1].position", pointLightPositions[1]);
    lightingShader.set_float("pointLights[1].constant", 1.0f);
    lightingShader.set_float("pointLights[1].linear", 0.09f);
    lightingShader.set_float("pointLights[1].quadratic", 0.032f);
    lightingShader.set_vec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
    lightingShader.set_vec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
    lightingShader.set_vec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);

    lightingShader.set_vec3("pointLights[2].position", pointLightPositions[2]);
    lightingShader.set_float("pointLights[2].constant", 1.0f);
    lightingShader.set_float("pointLights[2].linear", 0.09f);
    lightingShader.set_float("pointLights[2].quadratic", 0.032f);
    lightingShader.set_vec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
    lightingShader.set_vec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
    lightingShader.set_vec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);

    lightingShader.set_vec3("pointLights[3].position", pointLightPositions[3]);
    lightingShader.set_float("pointLights[3].constant", 1.0f);
    lightingShader.set_float("pointLights[3].linear", 0.09f);
    lightingShader.set_float("pointLights[3].quadratic", 0.032f);
    lightingShader.set_vec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
    lightingShader.set_vec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
    lightingShader.set_vec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);

    lightingShader.set_vec3("spotLight.position", camera.position);
    lightingShader.set_vec3("spotLight.direction", camera.front);
    lightingShader.set_float("spotLight.cutoff", glm::cos(glm::radians(12.5f)));
    lightingShader.set_float("spotLight.outercutoff", glm::cos(glm::radians(15.0f)));
    lightingShader.set_float("spotLight.constant", 1.0f);
    lightingShader.set_float("spotLight.linear", 0.09f);
    lightingShader.set_float("spotLight.quadratic", 0.032f);
    if (enableSpotLight) {
        lightingShader.set_vec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        lightingShader.set_vec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        lightingShader.set_vec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
    } else {
        lightingShader.set_vec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        lightingShader.set_vec3("spotLight.diffuse", 0.0f, 0.0f, 0.0f);
        lightingShader.set_vec3("spotLight.specular", 0.0f, 0.0f, 0.0f);
    }

    glm::mat4 model = glm::mat4(1.0f);

    glBindVertexArray(VAO);
    int ncubes = sizeof(cubePositions) / sizeof(glm::vec3);
    for (int i = 0; i < ncubes; ++i) {
        model = glm::mat4(1.0f);
        model = glm::translate(model, cubePositions[i]);
        float angle = 20.0f * i;
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        lightingShader.set_mat4("model", model);

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    lightShader.use();

    lightShader.set_mat4("projection", projection);
    lightShader.set_mat4("view", view);

    glBindVertexArray(lightVAO);
    int nplights = sizeof(pointLightPositions) / sizeof(glm::vec3);
    for (int i = 0; i < nplights; ++i) {
        model = glm::mat4(1.0f);
        model = glm::translate(model, pointLightPositions[i]);
        model = glm::scale(model, glm::vec3(0.2f));
        lightShader.set_mat4("model", model);

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    SDL_GL_SwapWindow(window);
}

int main(int argc, char *argv[])
{
    LGL_2 lgl_2(argc, argv);
    return lgl_2.run();
}
