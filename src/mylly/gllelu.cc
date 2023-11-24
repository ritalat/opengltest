#include "gllelu.hh"

#include "path.hh"

#include "glad/gl.h"
#include "SDL.h"

#include <cstdio>
#include <filesystem>
#include <stdexcept>
#include <string_view>

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

#define WINDOW_NAME "SDL OpenGL"

GLlelu::GLlelu(int argc, char *argv[], GLVersion glVersion):
    m_window(nullptr),
    m_context(nullptr)
{
    if (argc > 0) {
        Path exe = Path(argv[0]).filename();
        fprintf(stderr, "Starting %s (%s)\n", WINDOW_NAME, cpath(exe));
    } else {
        fprintf(stderr, "Starting %s\n", WINDOW_NAME);
    }

    SDL_version compiled;
    SDL_version linked;
    SDL_VERSION(&compiled);
    SDL_GetVersion(&linked);
    fprintf(stderr, "Compiled with: SDL %u.%u.%u\n",
                    compiled.major, compiled.minor, compiled.patch);
    fprintf(stderr, "Loaded: SDL %u.%u.%u\n",
                    linked.major, linked.minor, linked.patch);

    Path dataDir = datadir();
    if (dataDir.is_absolute()) {
        fprintf(stderr, "Data directory: %s\n", cpath(dataDir));
    } else {
        Path absolute = std::filesystem::weakly_canonical(dataDir);
        fprintf(stderr, "Data directory: %s (%s)\n", cpath(dataDir), cpath(absolute));
    }

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        throw std::runtime_error("Failed to init SDL");

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, GLVERSIONMAJOR(glVersion));
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, GLVERSIONMINOR(glVersion));
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);

    m_windowSize.width = WINDOW_WIDTH;
    m_windowSize.height = WINDOW_HEIGHT;

    m_window = SDL_CreateWindow(WINDOW_NAME,
                                SDL_WINDOWPOS_UNDEFINED,
                                SDL_WINDOWPOS_UNDEFINED,
                                WINDOW_WIDTH, WINDOW_HEIGHT,
                                SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
    if (!m_window)
        throw std::runtime_error("Failed to create SDL window");

    m_context = SDL_GL_CreateContext(m_window);
    if (!m_context)
        throw std::runtime_error("Failed to create OpenGL context");

    if (!gladLoadGL((GLADloadfunc) SDL_GL_GetProcAddress))
        throw std::runtime_error("Failed to load OpenGL functions");

    SDL_GL_GetDrawableSize(m_window, &m_fbSize.width, &m_fbSize.height);
    glViewport(0, 0, m_fbSize.width, m_fbSize.height);

    SDL_GL_SetSwapInterval(1);
}

GLlelu::~GLlelu()
{
    if (m_context)
        SDL_GL_DeleteContext(m_context);

    if (m_window)
        SDL_DestroyWindow(m_window);

    SDL_Quit();
}

void GLlelu::window_name(std::string_view name)
{
    SDL_SetWindowTitle(m_window, name.data());
}

void GLlelu::window_size(int w, int h)
{
    m_windowSize.width = w;
    m_windowSize.height = h;
    SDL_SetWindowSize(m_window, m_windowSize.width, m_windowSize.height);
    SDL_GL_GetDrawableSize(m_window, &m_fbSize.width, &m_fbSize.height);
    glViewport(0, 0, m_fbSize.width, m_fbSize.height);
}

int GLlelu::run()
{
    if (!m_window || !m_context)
        return EXIT_FAILURE;

    fprintf(stderr, "Window size: %dx%d\n", m_windowSize.width, m_windowSize.height);
    fprintf(stderr, "Drawable size: %dx%d\n", m_fbSize.width, m_fbSize.height);

    fprintf(stderr, "OpenGL vendor: %s\n", glGetString(GL_VENDOR));
    fprintf(stderr, "OpenGL renderer: %s\n", glGetString(GL_RENDERER));
    fprintf(stderr, "OpenGL version: %s\n", glGetString(GL_VERSION));
    fprintf(stderr, "OpenGL Shading Language version: %s\n",
                    glGetString(GL_SHADING_LANGUAGE_VERSION));

    SDL_ShowWindow(m_window);
    return main_loop();
}
