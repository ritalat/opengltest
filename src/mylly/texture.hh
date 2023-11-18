#pragma once

#include "glad/gl.h"

#include <string_view>

class Texture
{
public:
    Texture();
    ~Texture();
    Texture(const Texture &) = delete;
    Texture &operator=(const Texture &) = delete;
    bool load(std::string_view file, bool flip = true);
    void activate(int unit);
    void filtering(int mag = GL_LINEAR, int min = GL_LINEAR_MIPMAP_LINEAR);
    void wrapping(int s = GL_REPEAT, int t = GL_REPEAT);

    unsigned int id;
};
