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

#include <string_view>

Material::Material(std::string_view diffuse, std::string_view specular, std::string_view normal):
    diffuse(diffuse),
    specular(specular),
    normal(normal),
    shininess(64.0f)
{
}


Himmeli::Himmeli(std::string_view model, std::string_view diffuse, std::string_view specular, std::string_view normal):
    m_model(model),
    m_material(diffuse, specular, normal),
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
    shader.set_int("material.specular", 1);
    shader.set_int("material.normal", 2);
    shader.set_float("material.shininess", m_material.shininess);
    shader.set_mat4("model", m_translate * m_rotate * m_scale);
    m_material.diffuse.activate(0);
    m_material.specular.activate(1);
    m_material.normal.activate(2);
    glBindVertexArray(m_model.m_VAO);
    if (m_model.m_numIndices > 0) {
        glDrawElements(GL_TRIANGLES, m_model.m_numIndices, GL_UNSIGNED_INT, 0);
    } else {
        glDrawArrays(GL_TRIANGLES, 0, m_model.m_numVertices);
    }
    glBindVertexArray(0);
}

BasicHimmeli::BasicHimmeli(std::string_view model, BasicMaterial material):
    m_model(model),
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
    if (m_model.m_numIndices > 0) {
        glDrawElements(GL_TRIANGLES, m_model.m_numIndices, GL_UNSIGNED_INT, 0);
    } else {
        glDrawArrays(GL_TRIANGLES, 0, m_model.m_numVertices);
    }
    glBindVertexArray(0);
}
