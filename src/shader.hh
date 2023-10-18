#pragma once

#include "glm/glm.hpp"

#include <string_view>

class Shader
{
public:
    Shader() = default;

    bool load(const std::string_view vert, const std::string_view frag);
    void use();

    void setBool(const std::string_view name, bool value);
    void setInt(const std::string_view name, int value);
    void setFloat(const std::string_view name, float value);

    void setVec2(const std::string_view name, const glm::vec2 &value);
    void setVec2(const std::string_view name, float x, float y);
    void setVec3(const std::string_view name, const glm::vec3 &value);
    void setVec3(const std::string_view name, float x, float y, float z);
    void setVec4(const std::string_view name, const glm::vec4 &value);
    void setVec4(const std::string_view name, float x, float y, float z, float w);

    void setMat2(const std::string_view name, const glm::mat2 &value);
    void setMat3(const std::string_view name, const glm::mat3 &value);
    void setMat4(const std::string_view name, const glm::mat4 &value);

    unsigned int id;
};
