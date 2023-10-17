#pragma once

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

    unsigned int id;
};
