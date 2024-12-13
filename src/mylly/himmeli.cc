#include "himmeli.hh"

#include "model.hh"
#include "shader.hh"
#include "texture.hh"

#if defined(USE_GLES)
#include "glad/gles2.h"
#else
#include "glad/gl.h"
#endif
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <string_view>

HimmeliBase::HimmeliBase():
    m_scale(1.0f),
    m_rotate(1.0f),
    m_translate(1.0f)
{
}

HimmeliBase::~HimmeliBase()
{
}

void HimmeliBase::scale(const glm::mat4 &m, const glm::vec3 &v)
{
    m_scale = glm::scale(m, v);
}

void HimmeliBase::scale(const glm::vec3 &v)
{
    m_scale = glm::scale(m_scale, v);
}

void HimmeliBase::rotate(const glm::mat4 &m, const float angle, const glm::vec3 &v)
{
    m_rotate = glm::rotate(m, angle, v);
}

void HimmeliBase::rotate(const float angle, const glm::vec3 &v)
{
    m_rotate = glm::rotate(m_rotate, angle, v);
}

void HimmeliBase::translate(const glm::mat4 &m, const glm::vec3 &v)
{
    m_translate = glm::translate(m, v);
}

void HimmeliBase::translate(const glm::vec3 &v)
{
    m_translate = glm::translate(m_translate, v);
}

Material::Material(std::string_view diffuse, std::string_view specular, std::string_view normal):
    diffuse(diffuse),
    specular(specular),
    normal(normal),
    shininess(64.0f)
{
}

Himmeli::Himmeli(std::string_view model, std::string_view diffuse, std::string_view specular, std::string_view normal):
    m_model(model),
    m_material(diffuse, specular, normal)
{
}

// Assume same attributes and uniforms as lighting.{vert,frag}
void Himmeli::draw(Shader &shader)
{
    glm::mat4 model = m_translate * m_rotate * m_scale;
    shader.use();
    shader.set_int("material.diffuse", 0);
    shader.set_int("material.specular", 1);
    shader.set_int("material.normal", 2);
    shader.set_float("material.shininess", m_material.shininess);
    shader.set_mat4("model", model);
    shader.set_mat4("normalMat", glm::transpose(glm::inverse(model)));
    m_material.diffuse.activate(0);
    m_material.specular.activate(1);
    m_material.normal.activate(2);
    glBindVertexArray(m_model.vao());
    if (m_model.indices() > 0) {
        glDrawElements(GL_TRIANGLES, m_model.indices(), GL_UNSIGNED_INT, 0);
    } else {
        glDrawArrays(GL_TRIANGLES, 0, m_model.vertices());
    }
    glBindVertexArray(0);
}

BasicHimmeli::BasicHimmeli(std::string_view model, BasicMaterial material):
    m_model(model),
    m_material(material)
{
}

// Assume same attributes and uniforms as lighting{.vert,_basic.frag}
void BasicHimmeli::draw(Shader &shader)
{
    glm::mat4 model = m_translate * m_rotate * m_scale;
    shader.use();
    shader.set_vec3("material.ambient", m_material.ambient);
    shader.set_vec3("material.diffuse", m_material.diffuse);
    shader.set_vec3("material.specular", m_material.specular);
    shader.set_float("material.shininess", m_material.shininess);
    shader.set_mat4("model", model);
    shader.set_mat4("normalMat", glm::transpose(glm::inverse(model)));
    glBindVertexArray(m_model.vao());
    if (m_model.indices() > 0) {
        glDrawElements(GL_TRIANGLES, m_model.indices(), GL_UNSIGNED_INT, 0);
    } else {
        glDrawArrays(GL_TRIANGLES, 0, m_model.vertices());
    }
    glBindVertexArray(0);
}
