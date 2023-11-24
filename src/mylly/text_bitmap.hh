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
    TextRendererLatin1(int w, int h, std::string_view fontName);
    ~TextRendererLatin1();
    TextRendererLatin1(const TextRendererLatin1 &) = delete;
    TextRendererLatin1 &operator=(const TextRendererLatin1 &) = delete;
    void set_window_size(int w, int h);
    void set_scale(float scale);
    void set_color(glm::vec3 color);
    void set_color(float r, float g, float b);
    void draw_string(int x, int y, std::string_view str);

private:
    Shader m_textShader;
    Texture m_fontTexture;
    unsigned int m_textVAO;
    unsigned int m_textVBO;
    glm::vec3 m_color;
    float m_scale;
    int m_windowWidth;
    int m_windowHeight;
    glm::mat4 m_projection;
};
