#pragma once

#if defined(WIN32) && defined(NDEBUG)

#include "SDL.h"
#include <cstdio>
#include <cstdlib>
#include <exception>

#define GLLELU_MAIN_IMPLEMENTATION(Lelu)                                                        \
class Lelu;                                                                                     \
int main(int argc, char *argv[])                                                                \
{                                                                                               \
    try {                                                                                       \
        Lelu lelu(argc, argv);                                                                  \
        return lelu.run();                                                                      \
    } catch (const std::exception &e) {                                                         \
        printf("\nUnhandled exception: %s\n", e.what());                                        \
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Unhandled exception!", e.what(), NULL); \
        return EXIT_FAILURE;                                                                    \
    }                                                                                           \
}                                                                                               \

#else

#include <cstdio>
#include <cstdlib>
#include <exception>

#define GLLELU_MAIN_IMPLEMENTATION(Lelu)                          \
class Lelu;                                                       \
int main(int argc, char *argv[])                                  \
{                                                                 \
    try {                                                         \
        Lelu lelu(argc, argv);                                    \
        return lelu.run();                                        \
    } catch (const std::exception &e) {                           \
        fprintf(stderr, "\nUnhandled exception: %s\n", e.what()); \
        return EXIT_FAILURE;                                      \
    }                                                             \
}                                                                 \

#endif
