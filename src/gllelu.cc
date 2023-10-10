#include "gllelu.hh"

#include "glad/gl.h"
#include "SDL.h"

#include <stdio.h>
#include <stdlib.h>

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

GLlelu::GLlelu(int argc, char *argv[]):
    window(nullptr),
    context(nullptr),
    frameCount(0)
{
    (void)argc;
    (void)argv;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Failed to init SDL\n");
        return;
    }

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);

    window = SDL_CreateWindow("SDL OpenGL",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              WINDOW_WIDTH, WINDOW_HEIGHT,
                              SDL_WINDOW_OPENGL);
    if (!window) {
        fprintf(stderr, "Failed to create SDL window\n");
        return;
    }

    fprintf(stderr, "Window size: %ux%u\n", WINDOW_WIDTH, WINDOW_HEIGHT);

    context = SDL_GL_CreateContext(window);
    if (!context) {
        fprintf(stderr, "Failed to create OpenGL context\n");
        return;
    }

    SDL_GL_GetDrawableSize(window, &fbSize.width, &fbSize.height);
    fprintf(stderr, "Drawable size: %ux%u\n", fbSize.width, fbSize.height);

    gladLoadGL((GLADloadfunc) SDL_GL_GetProcAddress);
    fprintf(stderr, "OpenGL vendor: %s\n", glGetString(GL_VENDOR));
    fprintf(stderr, "OpenGL renderer: %s\n", glGetString(GL_RENDERER));
    fprintf(stderr, "OpenGL version: %s\n", glGetString(GL_VERSION));
    fprintf(stderr, "OpenGL Shading Language version: %s\n",
                    glGetString(GL_SHADING_LANGUAGE_VERSION));

    SDL_GL_SetSwapInterval(1);
    glViewport(0, 0, fbSize.width, fbSize.height);
}

GLlelu::~GLlelu()
{
    if (context)
        SDL_GL_DeleteContext(context);

    if (window)
        SDL_DestroyWindow(window);

    SDL_Quit();
}

int GLlelu::run()
{
    if (!window || !context)
        return EXIT_FAILURE;

    bool quit = false;
    SDL_Event event;

    while (!quit) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
                default:
                    break;
            }
        }

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        SDL_GL_SwapWindow(window);

        ++frameCount;
    }

    return EXIT_SUCCESS;
}
