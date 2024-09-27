#pragma once

#if defined(USE_GLES)
#include "glad/gles2.h"
#else
#include "glad/gl.h"
#endif

#include <array>
#include <string_view>

class Texture
{
public:
    Texture(std::string_view file, bool flip = true);
    ~Texture();
    Texture(const Texture &) = delete;
    Texture &operator=(const Texture &) = delete;
    void activate(int unit);
    void filtering(int mag = GL_LINEAR, int min = GL_LINEAR_MIPMAP_LINEAR);
    void wrapping(int s = GL_REPEAT, int t = GL_REPEAT);

    unsigned int m_id;
};

class Cubemap
{
public:
    Cubemap(const std::array<std::string_view, 6> &files);
    ~Cubemap();
    Cubemap(const Cubemap &) = delete;
    Cubemap &operator=(const Cubemap &) = delete;
    void activate(int unit);
    void filtering(int mag = GL_LINEAR, int min = GL_LINEAR);
    void wrapping(int s = GL_CLAMP_TO_EDGE, int t = GL_CLAMP_TO_EDGE, int r = GL_CLAMP_TO_EDGE);

    unsigned int m_id;
};
