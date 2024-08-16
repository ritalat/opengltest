#pragma once

#include "model.hh"
#include "shader.hh"
#include "texture.hh"

#include "glm/glm.hpp"

#include <string_view>

struct Material {
    Material(std::string_view diffuse,
             std::string_view specular,
             std::string_view normal);
    Texture diffuse;
    Texture specular;
    Texture normal;
    float shininess;
};

class Himmeli
{
public:
    Himmeli(std::string_view model, std::string_view diffuse,
                                    std::string_view specular = "none_specular.png",
                                    std::string_view normal = "none_normal.png");
    Himmeli(const Himmeli &) = delete;
    Himmeli &operator=(const Himmeli &) = delete;
    void draw(Shader &shader);

    Model m_model;
    Material m_material;
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
    BasicHimmeli(std::string_view model, BasicMaterial material);
    BasicHimmeli(const Himmeli &) = delete;
    BasicHimmeli &operator=(const Himmeli &) = delete;
    void draw(Shader &shader);

    Model m_model;
    BasicMaterial m_material;
    glm::mat4 m_scale;
    glm::mat4 m_rotate;
    glm::mat4 m_translate;
};
