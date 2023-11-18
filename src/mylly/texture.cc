#include "texture.hh"

#include "path.hh"

#include "glad/gl.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <string_view>

Texture::Texture():
    id(0)
{
}

Texture::~Texture()
{
    if (id)
        glDeleteTextures(1, &id);
}

void Texture::activate(int unit)
{
    if (unit < 48) {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, id);
    }
}

void Texture::filtering(int mag, int min)
{
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::wrapping(int s, int t)
{
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, s);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, t);
    glBindTexture(GL_TEXTURE_2D, 0);
}

auto channelsToFormat = [](int n){
    if (n == 1) {
        return GL_RED;
    } else if (n == 3) {
        return GL_RGB;
    } else {
        return GL_RGBA;
    }
};

bool Texture::load(std::string_view file, bool flip)
{
    stbi_set_flip_vertically_on_load(flip);

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    Path texturePath = get_asset_path(file);
    unsigned char *data = stbi_load(cpath(texturePath), &width, &height, &nrChannels, 0);

    if (data) {
        GLenum format = channelsToFormat(nrChannels);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
        return true;
    } else {
        fprintf(stderr, "Failed to load texture: %s\n", cpath(texturePath));
        return false;
    }
}
