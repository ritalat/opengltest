#pragma once

#include "model.hh"
#include "shader.hh"
#include "texture.hh"

#include "glm/glm.hpp"

#include <string_view>

class Himmeli
{
public:
    Himmeli();
    ~Himmeli();
    Himmeli(const Himmeli &) = delete;
    Himmeli &operator=(const Himmeli &) = delete;
    bool load(std::string_view modelFile, std::string_view textureFile);
    void draw(Shader &shader);

    Model model;
    Texture texture;
    float shininess;
    glm::mat4 scale;
    glm::mat4 rotate;
    glm::mat4 translate;
};
