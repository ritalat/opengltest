#include "gllelu.hh"
#include "gllelucamera.hh"

#include "SDL.h"

#include <cstddef>
#include <cstdio>

// FIXME: Move to es3_samples.hh or something
// Even if only Android needs them :/
class TriangleES3: public GLlelu
{
public:
    TriangleES3(int argc, char *argv[]);
    virtual ~TriangleES3();
    virtual int main_loop();
};

class BitmapFontES3: public GLlelu
{
public:
    BitmapFontES3(int argc, char *argv[]);
    virtual ~BitmapFontES3();
    virtual int main_loop();
};

class CubemapsES3: public GLleluCamera
{
public:
    CubemapsES3(int argc, char *argv[]);
    virtual ~CubemapsES3();
    virtual Status render();
};

const char *samples[] {
    "triangle",
    "bitmap_font",
    "cubemaps"
};

// FIXME: Make a less shit launcher at some point :)
int main(int argc, char *argv[])
{
    try {
        SDL_MessageBoxButtonData buttons[] = {
            { 0, -1, "prev" },
            { 0, 0, "launch" },
            { 0, 1, "next" },
        };
        SDL_MessageBoxData messageBoxData = {
            SDL_MESSAGEBOX_INFORMATION,
            NULL,
            "Select a sample",
            samples[0],
            SDL_arraysize(buttons),
            buttons,
            NULL
        };

        int buttonId = 1;
        int selectedSample = 0;

        while (buttonId != 0) {
            messageBoxData.message = samples[selectedSample];
            if (SDL_ShowMessageBox(&messageBoxData, &buttonId) != 0) {
                fprintf(stderr, "Failed to show message box\n");
                return EXIT_FAILURE;
            }
            if (buttonId == 1) {
                ++selectedSample;
                if (selectedSample >= sizeof(samples) / sizeof(char*))
                    selectedSample = 0;
            }
            if (buttonId == -1) {
                --selectedSample;
                if (selectedSample < 0)
                    selectedSample = sizeof(samples) / sizeof(char*) - 1;
            }
        }

        switch (selectedSample) {
            case 0: {
                TriangleES3 triangle(argc, argv);
                return triangle.run();
            }
            case 1: {
                BitmapFontES3 bitmapFont(argc, argv);
                return bitmapFont.run();
            }
            case 2: {
                CubemapsES3 cubemaps(argc, argv);
                return cubemaps.run();
            }
            default: {
                fprintf(stderr, "Unknown sample\n");
                return EXIT_FAILURE;
            }
        }
    } catch (const std::exception &e) {
        fprintf(stderr, "\nUnhandled exception: %s\n", e.what());
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Unhandled exception!", e.what(), NULL);
        return EXIT_FAILURE;
    }
}
