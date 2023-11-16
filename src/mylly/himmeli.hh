#pragma once

#include "model.hh"
#include "shader.hh"

#include "glm/glm.hpp"

#include <string_view>

class Himmeli
{
public:
    Himmeli();
    ~Himmeli();
    bool load(std::string_view modelFile, std::string_view textureFile);
    void draw(Shader &shader);

    Model model;
    unsigned int texture;
    float shininess;
    glm::mat4 scale;
    glm::mat4 rotate;
    glm::mat4 translate;
};
