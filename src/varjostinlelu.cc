#include "gllelu.hh"
#include "gllelu_main.hh"
#include "shader.hh"
#include "text_bitmap.hh"

#include "glad/gl.h"
#include "SDL.h"

#include <cstdio>
#include <cstdlib>
#include <optional>
#include <stdexcept>
#include <string>

class VarjostinLelu: public GLlelu
{
public:
    VarjostinLelu(int argc, char *argv[]);
    virtual ~VarjostinLelu();

protected:
    virtual int mainLoop();
    void loadShaders();

private:
    std::optional<Shader> m_leluShader;
    std::string m_error;
};

VarjostinLelu::VarjostinLelu(int argc, char *argv[]):
    GLlelu(argc, argv)
{
}

VarjostinLelu::~VarjostinLelu()
{
}

int VarjostinLelu::mainLoop()
{
    std::string notification;
    unsigned int notificationExpiration = 0;

    int width = fbSize().width;
    int height = fbSize().height;

    TextRendererLatin1 txt(width, height, "font8x8.png");

    loadShaders();

    unsigned int dummyVAO;
    glGenVertexArrays(1, &dummyVAO);

    glBindVertexArray(0);

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
                        notification = "Reloading shaders...";
                        notificationExpiration = SDL_GetTicks() + 2000;
                        loadShaders();
                    }
                    break;
                default:
                    break;
            }
        }

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        if (m_leluShader) {
            m_leluShader->use();

            m_leluShader->setVec2("u_resolution", static_cast<float>(width), static_cast<float>(height));
            int x, y;
            SDL_GetMouseState(&x, &y);
            m_leluShader->setVec2("u_mouse", static_cast<float>(x), static_cast<float>(height - y));
            m_leluShader->setFloat("u_time", static_cast<float>(SDL_GetTicks()) / 1000.0f);

            glBindVertexArray(dummyVAO);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            glBindVertexArray(0);
        }

        if (!m_error.empty()) {
            txt.setScale(1.0f);
            txt.setColor(1.0f, 0.0f, 0.0f);
            txt.drawString(0, 0, m_error);
        }

        if (notificationExpiration > SDL_GetTicks()) {
            float scale = 3.0f;
            txt.setScale(scale);
            txt.setColor(0.0f, 1.0f, 0.0f);
            txt.drawString(0, height - static_cast<int>(scale) * FONT_SIZE, notification);
        }

        swapWindow();
    }

    glDeleteVertexArrays(1, &dummyVAO);

    return EXIT_SUCCESS;
}

void VarjostinLelu::loadShaders()
{
    try {
        m_leluShader.emplace("varjostinlelu.vert", "varjostinlelu.frag");
        fprintf(stderr, "Varjostinlelu shaders loaded successfully\n");
        m_error.clear();
    } catch (const std::runtime_error &e) {
        fprintf(stderr, "Failed to load varjostinlelu shaders: %s\n", e.what());
        m_error = "Failed to compile varjostinlelu shaders!\n"
                  "\nException:\n"
                  + std::string(e.what()) +
                  "\nPress enter to reload.";
    }
}

GLLELU_MAIN_IMPLEMENTATION(VarjostinLelu)
