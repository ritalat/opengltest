#pragma once

#include "glm/glm.hpp"

#include <string>
#include <string_view>
#include <unordered_map>

class Shader
{
public:
    Shader(const std::string_view vert, const std::string_view frag);
#if !(defined(USE_GLES))
    Shader(const std::string_view comp);
#endif
    ~Shader();
    Shader(const Shader &) = delete;
    Shader &operator=(const Shader &) = delete;

    void use();

    void set_bool(const std::string_view name, bool value);
    void set_int(const std::string_view name, int value);
    void set_float(const std::string_view name, float value);

    void set_vec2(const std::string_view name, const glm::vec2 &value);
    void set_vec2(const std::string_view name, float x, float y);
    void set_vec3(const std::string_view name, const glm::vec3 &value);
    void set_vec3(const std::string_view name, float x, float y, float z);
    void set_vec4(const std::string_view name, const glm::vec4 &value);
    void set_vec4(const std::string_view name, float x, float y, float z, float w);

    void set_mat2(const std::string_view name, const glm::mat2 &value);
    void set_mat3(const std::string_view name, const glm::mat3 &value);
    void set_mat4(const std::string_view name, const glm::mat4 &value);

    int get_uniform_location(const std::string_view name);

    unsigned int m_id;
    std::unordered_map<std::string, int> m_uniformLocations;
};
