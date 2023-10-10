#include "gllelu.hh"

#include "SDL.h"

#include <stdio.h>

int main(int argc, char *argv[])
{
    SDL_version compiled;
    SDL_version linked;
    SDL_VERSION(&compiled);
    SDL_GetVersion(&linked);
    fprintf(stderr, "Compiled with: SDL %u.%u.%u\n",
                    compiled.major, compiled.minor, compiled.patch);
    fprintf(stderr, "Loaded: SDL %u.%u.%u\n",
                    linked.major, linked.minor, linked.patch);

    GLlelu gllelu(argc, argv);
    return gllelu.run();
}
