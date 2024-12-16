#pragma once

#include "api_decl.hh"
#include "model.hh"
#include "shader.hh"
#include "texture.hh"

#include "glm/glm.hpp"

#include <string_view>

class MYLLY_API HimmeliBase
{
public:
    HimmeliBase();
    virtual ~HimmeliBase();
    HimmeliBase(const HimmeliBase &) = delete;
    HimmeliBase &operator=(const HimmeliBase &) = delete;
    virtual void draw(Shader &shader) = 0;
    void scale(const glm::mat4 &m, const glm::vec3 &v);
    void scale(const glm::vec3 &v);
    void rotate(const glm::mat4 &m, const float angle, const glm::vec3 &v);
    void rotate(const float angle, const glm::vec3 &v);
    void translate(const glm::mat4 &m, const glm::vec3 &v);
    void translate(const glm::vec3 &v);

protected:
    glm::mat4 m_scale;
    glm::mat4 m_rotate;
    glm::mat4 m_translate;
};

struct Material {
    Material(std::string_view diffuse,
             std::string_view specular,
             std::string_view normal);
    Texture diffuse;
    Texture specular;
    Texture normal;
    float shininess;
};

class MYLLY_API Himmeli : public HimmeliBase
{
public:
    Himmeli(std::string_view model, std::string_view diffuse,
                                    std::string_view specular = "none_specular.png",
                                    std::string_view normal = "none_normal.png");
    virtual ~Himmeli();
    virtual void draw(Shader &shader);

private:
    Model m_model;
    Material m_material;
};

struct BasicMaterial {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
};

class MYLLY_API BasicHimmeli : public HimmeliBase
{
public:
    BasicHimmeli(std::string_view model, BasicMaterial material);
    virtual ~BasicHimmeli();
    virtual void draw(Shader &shader);

private:
    Model m_model;
    BasicMaterial m_material;
};
