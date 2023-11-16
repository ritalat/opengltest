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
    if (texture)
        glDeleteTextures(1, &texture);

    if (model.VAO)
        glDeleteVertexArrays(1, &model.VAO);

    if (model.VBO)
        glDeleteBuffers(1, &model.VBO);
}

bool Himmeli::load(std::string_view modelFile, std::string_view textureFile)
{
    if (!load_obj(model, modelFile))
        return false;

    if (!load_texture(texture, textureFile))
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
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(model.VAO);
    glDrawArrays(GL_TRIANGLES, 0, model.vertices.size());
}
