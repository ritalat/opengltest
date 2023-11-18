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
    Model();
    ~Model();
    Model(const Model &) = delete;
    Model &operator=(const Model &) = delete;
    void load_obj(std::string_view file);

    std::vector<Vertex> vertices;
    unsigned int VAO;
    unsigned int VBO;
};
