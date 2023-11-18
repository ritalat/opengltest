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
    Himmeli(const Himmeli &) = delete;
    Himmeli &operator=(const Himmeli &) = delete;
    void load(std::string_view modelFile, std::string_view textureFile);
    void draw(Shader &shader);

    Model model;
    Texture texture;
    float shininess;
    glm::mat4 scale;
    glm::mat4 rotate;
    glm::mat4 translate;
};

struct BasicMaterial {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
};

class BasicHimmeli
{
public:
    BasicHimmeli();
    BasicHimmeli(const Himmeli &) = delete;
    BasicHimmeli &operator=(const Himmeli &) = delete;
    void load(std::string_view modelFile, BasicMaterial material);
    void draw(Shader &shader);

    Model model;
    BasicMaterial material;
    glm::mat4 scale;
    glm::mat4 rotate;
    glm::mat4 translate;
};
