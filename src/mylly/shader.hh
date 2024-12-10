#pragma once

#include "glm/glm.hpp"

#include <string>
#include <string_view>
#include <unordered_map>

class Shader
{
public:
    Shader(std::string_view vert, std::string_view frag);
#if !(defined(USE_GLES))
    Shader(std::string_view comp);
#endif
    ~Shader();
    Shader(const Shader &) = delete;
    Shader &operator=(const Shader &) = delete;

    void use();
    unsigned int id() const;

    void set_bool(std::string_view name, bool value);
    void set_int(std::string_view name, int value);
    void set_float(std::string_view name, float value);

    void set_vec2(std::string_view name, const glm::vec2 &value);
    void set_vec2(std::string_view name, float x, float y);
    void set_vec3(std::string_view name, const glm::vec3 &value);
    void set_vec3(std::string_view name, float x, float y, float z);
    void set_vec4(std::string_view name, const glm::vec4 &value);
    void set_vec4(std::string_view name, float x, float y, float z, float w);

    void set_mat2(std::string_view name, const glm::mat2 &value);
    void set_mat3(std::string_view name, const glm::mat3 &value);
    void set_mat4(std::string_view name, const glm::mat4 &value);

private:
    int get_uniform_location(std::string_view name);

    unsigned int m_id;
    std::unordered_map<std::string, int> m_uniformLocations;
};
