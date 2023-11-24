#include "himmeli.hh"

#include "model.hh"
#include "shader.hh"
#include "texture.hh"

#include "glad/gl.h"
#include "glm/glm.hpp"

#include <string_view>

Himmeli::Himmeli(std::string_view modelFile, std::string_view textureFile):
    m_model(modelFile),
    m_texture(textureFile),
    m_shininess(64.0f),
    m_scale(1.0f),
    m_rotate(1.0f),
    m_translate(1.0f)
{
}

// Assume same attributes and uniforms as lighting.{vert,frag}
void Himmeli::draw(Shader &shader)
{
    shader.use();
    shader.set_int("material.diffuse", 0);
    shader.set_int("material.specular", 0);
    shader.set_float("material.shininess", m_shininess);
    shader.set_mat4("model", m_translate * m_rotate * m_scale);
    m_texture.activate(0);
    glBindVertexArray(m_model.m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, m_model.m_vertices.size());
}

BasicHimmeli::BasicHimmeli(std::string_view modelFile, BasicMaterial material):
    m_model(modelFile),
    m_material(material),
    m_scale(1.0f),
    m_rotate(1.0f),
    m_translate(1.0f)
{
}

// Assume same attributes and uniforms as lighting{.vert,_basic.frag}
void BasicHimmeli::draw(Shader &shader)
{
    shader.use();
    shader.set_vec3("material.ambient", m_material.ambient);
    shader.set_vec3("material.diffuse", m_material.diffuse);
    shader.set_vec3("material.specular", m_material.specular);
    shader.set_float("material.shininess", m_material.shininess);
    shader.set_mat4("model", m_translate * m_rotate * m_scale);
    glBindVertexArray(m_model.m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, m_model.m_vertices.size());
}
