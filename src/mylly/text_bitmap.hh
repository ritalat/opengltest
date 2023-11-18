#pragma once

#include "shader.hh"
#include "texture.hh"

#include "glm/glm.hpp"

#include <string_view>

#define FONT_NAME "font8x8.png"
#define FONT_SIZE 8
#define ATLAS_SIZE 16

class TextRendererLatin1
{
public:
    TextRendererLatin1(int w, int h);
    ~TextRendererLatin1();
    TextRendererLatin1(const TextRendererLatin1 &) = delete;
    TextRendererLatin1 &operator=(const TextRendererLatin1 &) = delete;
    bool load_font(std::string_view fontName);
    void set_window_size(int w, int h);
    void set_scale(float scale);
    void set_color(glm::vec3 color);
    void set_color(float r, float g, float b);
    void draw_string(int x, int y, std::string_view str);

private:
    Shader textShader;
    unsigned int textVBO;
    unsigned int textVAO;
    Texture fontTexture;
    glm::mat4 projection;
    glm::vec3 color;
    float scale;
    int windowWidth;
    int windowHeight;
};
