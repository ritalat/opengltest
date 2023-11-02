#include "texture.hh"

#include "path.hh"

#include "glad/gl.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <string_view>

auto channelsToFormat = [](int n){
    if (n == 1) {
        return GL_RED;
    } else if (n == 3) {
        return GL_RGB;
    } else {
        return GL_RGBA;
    }
};

bool load_texture(unsigned int &textureId, std::string_view file)
{
    stbi_set_flip_vertically_on_load(true);

    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    Path texturePath = get_asset_path(file);
    unsigned char *data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);

    if (data) {
        GLenum format = channelsToFormat(nrChannels);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
        return true;
    } else {
        fprintf(stderr, "Failed to load texture: %s\n", texturePath.c_str());
        return false;
    }
}
