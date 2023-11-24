#include "text_bitmap.hh"

#include "shader.hh"
#include "texture.hh"

#include "glad/gl.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <cmath>
#include <cstdint>
#include <string_view>

// UTF8_getch function taken from SDL2's SDL_test_font.c
// The included font data based on Marcel Sondaar's font8_8.asm was dumped to font8x8.png

#define UNKNOWN_UNICODE 0xFFFD
#define UTF8_IsTrailingByte(c) ((c) >= 0x80 && (c) <= 0xBF)

static uint32_t UTF8_getch(const char *src, size_t srclen, int *inc)
{
    const uint8_t *p = (const uint8_t *)src;
    size_t left = 0;
    size_t save_srclen = srclen;
    bool overlong = false;
    bool underflow = false;
    uint32_t ch = UNKNOWN_UNICODE;

    if (srclen == 0) {
        return UNKNOWN_UNICODE;
    }
    if (p[0] >= 0xFC) {
        if ((p[0] & 0xFE) == 0xFC) {
            if (p[0] == 0xFC && (p[1] & 0xFC) == 0x80) {
                overlong = true;
            }
            ch = (uint32_t)(p[0] & 0x01);
            left = 5;
        }
    } else if (p[0] >= 0xF8) {
        if ((p[0] & 0xFC) == 0xF8) {
            if (p[0] == 0xF8 && (p[1] & 0xF8) == 0x80) {
                overlong = true;
            }
            ch = (uint32_t)(p[0] & 0x03);
            left = 4;
        }
    } else if (p[0] >= 0xF0) {
        if ((p[0] & 0xF8) == 0xF0) {
            if (p[0] == 0xF0 && (p[1] & 0xF0) == 0x80) {
                overlong = true;
            }
            ch = (uint32_t)(p[0] & 0x07);
            left = 3;
        }
    } else if (p[0] >= 0xE0) {
        if ((p[0] & 0xF0) == 0xE0) {
            if (p[0] == 0xE0 && (p[1] & 0xE0) == 0x80) {
                overlong = true;
            }
            ch = (uint32_t)(p[0] & 0x0F);
            left = 2;
        }
    } else if (p[0] >= 0xC0) {
        if ((p[0] & 0xE0) == 0xC0) {
            if ((p[0] & 0xDE) == 0xC0) {
                overlong = true;
            }
            ch = (uint32_t)(p[0] & 0x1F);
            left = 1;
        }
    } else {
        if (!(p[0] & 0x80)) {
            ch = (uint32_t)p[0];
        }
    }
    --srclen;
    while (left > 0 && srclen > 0) {
        ++p;
        if ((p[0] & 0xC0) != 0x80) {
            ch = UNKNOWN_UNICODE;
            break;
        }
        ch <<= 6;
        ch |= (p[0] & 0x3F);
        --srclen;
        --left;
    }
    if (left > 0) {
        underflow = true;
    }

    if (overlong || underflow ||
        (ch >= 0xD800 && ch <= 0xDFFF) ||
        (ch == 0xFFFE || ch == 0xFFFF) || ch > 0x10FFFF) {
        ch = UNKNOWN_UNICODE;
    }

    *inc = (int)(save_srclen - srclen);

    return ch;
}

TextRendererLatin1::TextRendererLatin1(int w, int h, std::string_view fontName):
    m_textShader("text_bitmap.vert", "text_bitmap.frag"),
    m_fontTexture(fontName, false),
    m_textVAO(0),
    m_textVBO(0),
    m_color(1.0f, 1.0f, 1.0f),
    m_scale(1.0f),
    m_windowWidth(w),
    m_windowHeight(h),
    m_projection(glm::ortho(0.0f, (float)w, 0.0f, (float)h))
{
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    m_fontTexture.filtering(GL_NEAREST);

    glGenVertexArrays(1, &m_textVAO);
    glBindVertexArray(m_textVAO);

    glGenBuffers(1, &m_textVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_textVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
}

TextRendererLatin1::~TextRendererLatin1()
{
    if (m_textVAO)
        glDeleteVertexArrays(1, &m_textVAO);

    if (m_textVBO)
        glDeleteBuffers(1, &m_textVBO);
}

void TextRendererLatin1::set_window_size(int w, int h)
{
    m_windowWidth = w;
    m_windowHeight = h;
    m_projection = glm::ortho(0.0f, (float)w, 0.0f, (float)h);
}

void TextRendererLatin1::set_scale(float scale)
{
    m_scale = scale;
}

void TextRendererLatin1::set_color(glm::vec3 color)
{
    m_color = color;
}

void TextRendererLatin1::set_color(float r, float g, float b)
{
    m_color = glm::vec3(r, g, b);
}

// Top left corner of the window is treated as (0,0) to make drawing text more natural
void TextRendererLatin1::draw_string(int x, int y, std::string_view str)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_fontTexture.activate(0);

    glBindVertexArray(m_textVAO);

    m_textShader.use();
    m_textShader.set_mat4("projection", m_projection);
    m_textShader.set_vec3("color", m_color);
    m_textShader.set_int("font", 0);

    float curx = x;
    float cury = m_windowHeight - y - m_scale * FONT_SIZE;
    float w = m_scale * FONT_SIZE;
    float h = m_scale * FONT_SIZE;

    const char *s = str.data();
    size_t len = str.length();

    while (len > 0) {
        int advance = 0;
        uint32_t ch = UTF8_getch(s, len, &advance);

        if (ch == 0x0A) {
            // Newline
            curx = x;
            cury -= m_scale * FONT_SIZE;
        } else if (ch == 0x09) {
            // Tab
            curx += m_scale * FONT_SIZE * 4;
        } else if (ch == 0x20) {
            // Space
            curx += m_scale * FONT_SIZE;
        } else if (ch <= 0xFF) {
            float vertices[6][4] = {
                { curx,     cury + h,  0.0f, 0.0f },
                { curx,     cury,      0.0f, 1.0f },
                { curx + w, cury,      1.0f, 1.0f },
                { curx,     cury + h,  0.0f, 0.0f },
                { curx + w, cury,      1.0f, 1.0f },
                { curx + w, cury + h,  1.0f, 0.0f }
            };

            glBindBuffer(GL_ARRAY_BUFFER, m_textVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            float offx = floor(ch % ATLAS_SIZE) / ATLAS_SIZE;
            float offy = floor(ch / ATLAS_SIZE) / ATLAS_SIZE;
            float scalex = 1.0f / ATLAS_SIZE;
            float scaley = 1.0f / ATLAS_SIZE;
            m_textShader.set_vec2("offset", offx, offy);
            m_textShader.set_vec2("scale", scalex, scaley);

            glDrawArrays(GL_TRIANGLES, 0, 6);

            curx += m_scale * FONT_SIZE;
        }

        s += advance;
        len -= advance;
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_BLEND);
}
