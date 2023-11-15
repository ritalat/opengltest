#pragma once

#include "glm/glm.hpp"

#include <string_view>
#include <vector>

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texcoord;
};

struct Model
{
    std::vector<Vertex> vertices;
    unsigned int VAO;
    unsigned int VBO;
};

bool load_obj(Model &model, std::string_view file);
