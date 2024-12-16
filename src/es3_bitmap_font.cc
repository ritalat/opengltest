#include "gllelu.hh"
#include "gllelu_main.hh"
#include "shader.hh"
#include "text_bitmap.hh"

#if defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>
#endif
#include "glad/gles2.h"
#include "SDL.h"

#include <cmath>
#include <cstdlib>
#include <string>

class BitmapFontES3: public GLlelu
{
public:
    BitmapFontES3(int argc, char *argv[]);
    virtual ~BitmapFontES3();

protected:
    virtual int mainLoop();
    void iterate();

private:
    TextRendererLatin1 m_txt;
    std::string m_fontLoaded;
    std::string m_glInfoDump;
    std::string m_aakkosia;
    bool m_quit;
};

#if defined(__EMSCRIPTEN__)
void browserCallback(void *arg)
{
    static_cast<BitmapFontES3*>(arg)->iterate();
}
#endif

BitmapFontES3::BitmapFontES3(int argc, char *argv[]):
    GLlelu(argc, argv),
    m_txt(fbSize().width, fbSize().height, "font8x8.png"),
    m_fontLoaded("Loaded font font8x8.png"),
    m_glInfoDump("OpenGL vendor: " + std::string((char *)glGetString(GL_VENDOR)) + '\n' +
                 "OpenGL renderer: " + std::string((char *)glGetString(GL_RENDERER)) + '\n' +
                 "OpenGL version: " + std::string((char *)glGetString(GL_VERSION)) + '\n' +
                 "OpenGL Shading Language version: " + std::string((char *)glGetString(GL_SHADING_LANGUAGE_VERSION))),
    m_aakkosia("Ääkkösetkin toimii :---DD"),
    m_quit(false)
{
}

BitmapFontES3::~BitmapFontES3()
{
}

int BitmapFontES3::mainLoop()
{
#if defined(__EMSCRIPTEN__)
    emscripten_set_main_loop_arg(browserCallback, this, 0, 1);
#else
    while (!m_quit) {
        iterate();
    }
#endif

    return EXIT_SUCCESS;
}

void BitmapFontES3::iterate()
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                m_quit = true;
                break;
            case SDL_KEYUP:
                if (SDL_SCANCODE_ESCAPE == event.key.keysym.scancode)
                    m_quit = true;
                break;
            default:
                break;
        }
    }

#if defined(__EMSCRIPTEN__)
    if(m_quit)
        emscripten_cancel_main_loop();
#endif

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    int width = fbSize().width;
    int height = fbSize().height;

    m_txt.setColor(1.0f, 1.0f, 1.0f);
    m_txt.setScale(1.0f);
    m_txt.drawString(0, 0, m_glInfoDump);
    m_txt.drawString(width - static_cast<int>(m_fontLoaded.length()) * FONT_SIZE,
                     height - FONT_SIZE, m_fontLoaded);

    m_txt.setColor(1.0f, 0.0f, 0.0f);
    m_txt.setScale(2.5f);
    m_txt.drawString(250 + static_cast<int>(100.0f * sin(static_cast<float>(SDL_GetTicks()) / 1000.0f)),
                     height / 2, m_aakkosia);

    int x, y;
    unsigned int buttons = SDL_GetMouseState(&x, &y);
    if (buttons & SDL_BUTTON(SDL_BUTTON_LEFT)) {
        m_txt.setColor(0.0f, 1.0f, 0.0f);
    } else if (buttons & SDL_BUTTON(SDL_BUTTON_RIGHT)) {
        m_txt.setColor(0.0f, 0.0f, 1.0f);
    } else if (buttons & SDL_BUTTON(SDL_BUTTON_MIDDLE)) {
        m_txt.setColor(1.0f, 0.0f, 0.0f);
    } else {
        m_txt.setColor(1.0f, 1.0f, 1.0f);
    }
    m_txt.setScale(1.0f);
    std::string mouse = "Mouse state: (" + std::to_string(x) + "," + std::to_string(y) + ")";
    m_txt.drawString(0, height - FONT_SIZE, mouse);

    swapWindow();
}

GLLELU_MAIN_IMPLEMENTATION(BitmapFontES3)
