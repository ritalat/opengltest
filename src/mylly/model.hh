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

class Model
{
public:
    Model(std::string_view file);
    ~Model();
    Model(const Model &) = delete;
    Model &operator=(const Model &) = delete;

    std::vector<Vertex> m_vertices;
    unsigned int m_VAO;
    unsigned int m_VBO;
};
