#include "gllelu.hh"
#include "gllelu_main.hh"
#include "shader.hh"
#include "text_bitmap.hh"

#include "glad/gl.h"
#include "SDL3/SDL.h"

#include <cstdint>
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
    virtual SDL_AppResult event(SDL_Event *event);
    virtual SDL_AppResult iterate();
    void loadShaders();

private:
    std::optional<Shader> m_leluShader;
    std::string m_error;
    std::string m_notification;
    uint64_t m_notificationExpiration;
    TextRendererLatin1 m_txt;
    unsigned int m_dummyVAO;
};

VarjostinLelu::VarjostinLelu(int argc, char *argv[]):
    GLlelu(argc, argv),
    m_notificationExpiration(0),
    m_txt(fbSize().width, fbSize().height, "font8x8.png")
{
    loadShaders();

    glGenVertexArrays(1, &m_dummyVAO);
    glBindVertexArray(0);
}

VarjostinLelu::~VarjostinLelu()
{
    glDeleteVertexArrays(1, &m_dummyVAO);
}

SDL_AppResult VarjostinLelu::event(SDL_Event *event)
{
    switch (event->type) {
        case SDL_EVENT_KEY_UP:
            if (SDL_SCANCODE_RETURN == event->key.scancode) {
                m_notification = "Reloading shaders...";
                m_notificationExpiration = SDL_GetTicks() + 2000;
                loadShaders();
            }
            break;
        case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
            m_txt.setWindowSize(fbSize().width, fbSize().height);
            break;
        default:
            break;
    }

    return GLlelu::event(event);
}

SDL_AppResult VarjostinLelu::iterate()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    int width = fbSize().width;
    int height = fbSize().height;

    if (m_leluShader) {
        m_leluShader->use();

        m_leluShader->setVec2("u_resolution", static_cast<float>(width), static_cast<float>(height));
        float x, y;
        SDL_GetMouseState(&x, &y);
        m_leluShader->setVec2("u_mouse", x, static_cast<float>(height) - y);
        m_leluShader->setFloat("u_time", static_cast<float>(SDL_GetTicks()) / 1000.0f);

        glBindVertexArray(m_dummyVAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);
    }

    if (!m_error.empty()) {
        m_txt.setScale(1.0f);
        m_txt.setColor(1.0f, 0.0f, 0.0f);
        m_txt.drawString(0, 0, m_error);
    }

    if (m_notificationExpiration > SDL_GetTicks()) {
        float scale = 3.0f;
        m_txt.setScale(scale);
        m_txt.setColor(0.0f, 1.0f, 0.0f);
        m_txt.drawString(0, height - static_cast<int>(scale) * FONT_SIZE, m_notification);
    }

    return GLlelu::iterate();
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
