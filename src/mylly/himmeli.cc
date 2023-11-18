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

Himmeli::~Himmeli()
{
}

bool Himmeli::load(std::string_view modelFile, std::string_view textureFile)
{
    if (!model.load_obj(modelFile))
        return false;

    if (!texture.load(textureFile))
        return false;

    return true;
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
