#include "himmeli.hh"

#include "model.hh"
#include "shader.hh"
#include "texture.hh"

#include "glad/gl.h"
#include "glm/glm.hpp"

#include <string_view>

Himmeli::Himmeli():
    scale(1.0f),
    rotate(1.0f),
    translate(1.0f),
    shininess(64.0f)
{
}

void Himmeli::load(std::string_view modelFile, std::string_view textureFile)
{
    model.load_obj(modelFile);
    texture.load(textureFile);
}

// Assume same attributes and uniforms as lighting.{vert,frag}
void Himmeli::draw(Shader &shader)
{
    shader.use();
    shader.set_int("material.diffuse", 0);
    shader.set_int("material.specular", 0);
    shader.set_float("material.shininess", shininess);
    shader.set_mat4("model", translate * rotate * scale);
    texture.activate(0);
    glBindVertexArray(model.VAO);
    glDrawArrays(GL_TRIANGLES, 0, model.vertices.size());
}

BasicHimmeli::BasicHimmeli():
    scale(1.0f),
    rotate(1.0f),
    translate(1.0f)
{
}

void BasicHimmeli::load(std::string_view modelFile, BasicMaterial material)
{
    model.load_obj(modelFile);
    this->material = material;
}

// Assume same attributes and uniforms as lighting{.vert,_basic.frag}
void BasicHimmeli::draw(Shader &shader)
{
    shader.use();
    shader.set_vec3("material.ambient", material.ambient);
    shader.set_vec3("material.diffuse", material.diffuse);
    shader.set_vec3("material.specular", material.specular);
    shader.set_float("material.shininess", material.shininess);
    shader.set_mat4("model", translate * rotate * scale);
    glBindVertexArray(model.VAO);
    glDrawArrays(GL_TRIANGLES, 0, model.vertices.size());
}
