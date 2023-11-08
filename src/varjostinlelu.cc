#include "gllelu.hh"
#include "shader.hh"
#include "text_bitmap.hh"

#include "glad/gl.h"
#include "SDL.h"

#include <cstdlib>
#include <string>

const float vertices[] = {
     1.0f,  1.0f, 0.0f,
     1.0f, -1.0f, 0.0f,
    -1.0f, -1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f
};

const unsigned int indices[] = {
    0, 1, 3,
    1, 2, 3
};

class VarjostinLelu: public GLlelu
{
public:
    VarjostinLelu(int argc, char *argv[]);
    virtual ~VarjostinLelu();
    virtual int main_loop();
};

VarjostinLelu::VarjostinLelu(int argc, char *argv[]):
    GLlelu(argc, argv)
{
}

VarjostinLelu::~VarjostinLelu()
{
}

int VarjostinLelu::main_loop()
{
    std::string error;
    bool shaderCompiled = true;
    std::string notification;
    unsigned int notificationExpiration = 0;

    TextRendererLatin1 txt(fbSize.width, fbSize.height);
    if (!txt.load_font("font8x8.png"))
        return EXIT_FAILURE;

    Shader leluShader;
    if (!leluShader.load("varjostinlelu.vert", "varjostinlelu.frag")) {
        shaderCompiled = false;
        error = "Failed to compile varjostinlelu shaders!\n"
                "See the console log for more details.";
    }

    unsigned int leluVAO;
    glGenVertexArrays(1, &leluVAO);
    glBindVertexArray(leluVAO);

    unsigned int leluVBO;
    glGenBuffers(1, &leluVBO);
    glBindBuffer(GL_ARRAY_BUFFER, leluVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int leluEBO;
    glGenBuffers(1, &leluEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, leluEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    bool quit = false;
    SDL_Event event;

    while (!quit) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_KEYUP:
                    if (SDL_SCANCODE_ESCAPE == event.key.keysym.scancode)
                        quit = true;

                    if (SDL_SCANCODE_RETURN == event.key.keysym.scancode) {
                        notification = "Shader reloading not yet implemented :^)";
                        notificationExpiration = SDL_GetTicks() + 5000;
                    }
                    break;
                default:
                    break;
            }
        }

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        if (shaderCompiled) {
            leluShader.use();

            leluShader.set_vec2("u_resolution", (float)fbSize.width, (float)fbSize.height);
            int x, y;
            SDL_GetMouseState(&x, &y);
            leluShader.set_vec2("u_mouse", (float)x, (float)(fbSize.height - y));
            leluShader.set_float("u_time", (float)(SDL_GetTicks() / 1000.0f));

            glBindVertexArray(leluVAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }

        if (!error.empty()) {
            txt.set_scale(1.0f);
            txt.set_color(1.0f, 0.0f, 0.0f);
            txt.draw_string(0, 0, error);
        }

        if (notificationExpiration > SDL_GetTicks()) {
            float scale = 3.0f;
            txt.set_scale(scale);
            txt.set_color(0.0f, 1.0f, 0.0f);
            txt.draw_string(0, fbSize.height - scale * FONT_SIZE, notification);
        }

        SDL_GL_SwapWindow(window);
    }

    glDeleteBuffers(1, &leluEBO);
    glDeleteBuffers(1, &leluVBO);
    glDeleteVertexArrays(1, &leluVAO);

    return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
    VarjostinLelu lelu(argc, argv);
    return lelu.run();
}
