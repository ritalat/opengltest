#pragma once

#include "shader.hh"
#include "texture.hh"

#include "glm/glm.hpp"

#include <string_view>
#include <vector>

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
    void setWindowSize(int w, int h);
    void setScale(float scale);
    void setColor(glm::vec3 color);
    void setColor(float r, float g, float b);
    void drawString(int x, int y, std::string_view str);

private:
    void drawBatch(int size);

    Shader m_textShader;
    Texture m_fontTexture;
    unsigned int m_textVAO;
    unsigned int m_textVBO;
    glm::vec3 m_color;
    float m_scale;
    float m_windowWidth;
    float m_windowHeight;
    glm::mat4 m_projection;
    glm::mat2 m_texScale;
    std::vector<float> m_strVerts;
};
