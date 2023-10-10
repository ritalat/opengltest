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
    ~GLlelu();
    int run();
private:
    SDL_Window *window;
    SDL_GLContext context;
    DrawableSize fbSize;
    int frameCount;
};
