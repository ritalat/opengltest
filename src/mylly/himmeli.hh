#pragma once

#include "model.hh"
#include "shader.hh"
#include "texture.hh"

#include "glm/glm.hpp"

#include <string_view>

class Himmeli
{
public:
    Himmeli(std::string_view modelFile, std::string_view textureFile);
    Himmeli(const Himmeli &) = delete;
    Himmeli &operator=(const Himmeli &) = delete;
    void draw(Shader &shader);

    Model m_model;
    Texture m_texture;
    float m_shininess;
    glm::mat4 m_scale;
    glm::mat4 m_rotate;
    glm::mat4 m_translate;
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
    BasicHimmeli(std::string_view modelFile, BasicMaterial material);
    BasicHimmeli(const Himmeli &) = delete;
    BasicHimmeli &operator=(const Himmeli &) = delete;
    void draw(Shader &shader);

    Model m_model;
    BasicMaterial m_material;
    glm::mat4 m_scale;
    glm::mat4 m_rotate;
    glm::mat4 m_translate;
};
