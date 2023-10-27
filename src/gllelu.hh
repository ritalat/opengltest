#pragma once

#include "SDL.h"

struct DrawableSize
{
    int width;
    int height;
};

class GLlelu
{
public:
    GLlelu(int argc, char *argv[]);
    virtual ~GLlelu();
    int run();
    virtual int main_loop() = 0;

    SDL_Window *window;
    SDL_GLContext context;
    DrawableSize fbSize;
};
