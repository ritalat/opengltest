#include "gllelu.hh"
#include "gllelu_main.hh"
#include "shader.hh"
#include "text_bitmap.hh"

#include "glad/gl.h"
#include "SDL.h"

#include <cmath>
#include <cstdlib>
#include <string>

class BitmapFont: public GLlelu
{
public:
    BitmapFont(int argc, char *argv[]);
    virtual ~BitmapFont();

protected:
    virtual int mainLoop();
};

BitmapFont::BitmapFont(int argc, char *argv[]):
    GLlelu(argc, argv)
{
}

BitmapFont::~BitmapFont()
{
}

int BitmapFont::mainLoop()
{
    TextRendererLatin1 txt(fbSize().width, fbSize().height, "font8x8.png");

    std::string fontLoaded = "Loaded font font8x8.png";

    std::string glInfoDump = "OpenGL vendor: " + std::string((char *)glGetString(GL_VENDOR)) + '\n' +
                             "OpenGL renderer: " + std::string((char *)glGetString(GL_RENDERER)) + '\n' +
                             "OpenGL version: " + std::string((char *)glGetString(GL_VERSION)) + '\n' +
                             "OpenGL Shading Language version: " + std::string((char *)glGetString(GL_SHADING_LANGUAGE_VERSION));

    std::string aakkosia = "Ääkkösetkin toimii :---DD";

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
                    break;
                default:
                    break;
            }
        }

        int width = fbSize().width;
        int height = fbSize().height;

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        txt.setColor(1.0f, 1.0f, 1.0f);
        txt.setScale(1.0f);
        txt.drawString(0, 0, glInfoDump);
        txt.drawString(width - static_cast<int>(fontLoaded.length()) * FONT_SIZE,
                       height - FONT_SIZE, fontLoaded);

        txt.setColor(1.0f, 0.0f, 0.0f);
        txt.setScale(2.5f);
        txt.drawString(250 + static_cast<int>(100.0f * sin(static_cast<float>(SDL_GetTicks()) / 1000.0f)),
                       height / 2, aakkosia);

        int x, y;
        unsigned int buttons = SDL_GetMouseState(&x, &y);
        if (buttons & SDL_BUTTON(SDL_BUTTON_LEFT)) {
            txt.setColor(0.0f, 1.0f, 0.0f);
        } else if (buttons & SDL_BUTTON(SDL_BUTTON_RIGHT)) {
            txt.setColor(0.0f, 0.0f, 1.0f);
        } else if (buttons & SDL_BUTTON(SDL_BUTTON_MIDDLE)) {
            txt.setColor(1.0f, 0.0f, 0.0f);
        } else {
            txt.setColor(1.0f, 1.0f, 1.0f);
        }
        txt.setScale(1.0f);
        std::string mouse = "Mouse state: (" + std::to_string(x) + "," + std::to_string(y) + ")";
        txt.drawString(0, height - FONT_SIZE, mouse);

        swapWindow();
    }

    return EXIT_SUCCESS;
}

GLLELU_MAIN_IMPLEMENTATION(BitmapFont)
