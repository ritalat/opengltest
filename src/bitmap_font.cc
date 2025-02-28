#include "gllelu.hh"
#include "gllelu_main.hh"
#include "shader.hh"
#include "text_bitmap.hh"

#include "glad/gl.h"
#include "SDL3/SDL.h"

#include <cmath>
#include <cstdlib>
#include <string>

class BitmapFont: public GLlelu
{
public:
    BitmapFont(int argc, char *argv[]);
    virtual ~BitmapFont();
    virtual SDL_AppResult event(SDL_Event *event);
    virtual SDL_AppResult iterate();

private:
    TextRendererLatin1 m_txt;
};

BitmapFont::BitmapFont(int argc, char *argv[]):
    GLlelu(argc, argv),
    m_txt(fbSize().width, fbSize().height, "font8x8.png")
{
}

BitmapFont::~BitmapFont()
{
}

SDL_AppResult BitmapFont::event(SDL_Event *event)
{
    if (event->type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED) {
        m_txt.setWindowSize(fbSize().width, fbSize().height);
    }

    return GLlelu::event(event);
}

SDL_AppResult BitmapFont::iterate()
{
    std::string fontLoaded = "Loaded font font8x8.png";

    std::string glInfoDump = "OpenGL vendor: " + std::string((char *)glGetString(GL_VENDOR)) + '\n' +
                             "OpenGL renderer: " + std::string((char *)glGetString(GL_RENDERER)) + '\n' +
                             "OpenGL version: " + std::string((char *)glGetString(GL_VERSION)) + '\n' +
                             "OpenGL Shading Language version: " + std::string((char *)glGetString(GL_SHADING_LANGUAGE_VERSION));

    std::string aakkosia = "Ääkkösetkin toimii :---DD";

    int width = fbSize().width;
    int height = fbSize().height;

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    m_txt.setColor(1.0f, 1.0f, 1.0f);
    m_txt.setScale(1.0f);
    m_txt.drawString(0, 0, glInfoDump);
    m_txt.drawString(width - static_cast<int>(fontLoaded.length()) * FONT_SIZE,
                     height - FONT_SIZE, fontLoaded);

    m_txt.setColor(1.0f, 0.0f, 0.0f);
    m_txt.setScale(2.5f);
    m_txt.drawString(250 + static_cast<int>(100.0f * sin(static_cast<float>(SDL_GetTicks()) / 1000.0f)),
                     height / 2, aakkosia);

    float x, y;
    unsigned int buttons = SDL_GetMouseState(&x, &y);
    if (buttons & SDL_BUTTON_MASK(SDL_BUTTON_LEFT)) {
        m_txt.setColor(0.0f, 1.0f, 0.0f);
    } else if (buttons & SDL_BUTTON_MASK(SDL_BUTTON_RIGHT)) {
        m_txt.setColor(0.0f, 0.0f, 1.0f);
    } else if (buttons & SDL_BUTTON_MASK(SDL_BUTTON_MIDDLE)) {
        m_txt.setColor(1.0f, 0.0f, 0.0f);
    } else {
        m_txt.setColor(1.0f, 1.0f, 1.0f);
    }
    m_txt.setScale(1.0f);
    std::string mouse = "Mouse state: (" + std::to_string(static_cast<int>(x))
                        + "," + std::to_string(static_cast<int>(y)) + ")";
    m_txt.drawString(0, height - FONT_SIZE, mouse);

    return GLlelu::iterate();
}

GLLELU_MAIN_IMPLEMENTATION(BitmapFont)
