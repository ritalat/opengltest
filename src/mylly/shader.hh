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

    void setBool(std::string_view name, bool value);
    void setInt(std::string_view name, int value);
    void setFloat(std::string_view name, float value);

    void setVec2(std::string_view name, const glm::vec2 &value);
    void setVec2(std::string_view name, float x, float y);
    void setVec3(std::string_view name, const glm::vec3 &value);
    void setVec3(std::string_view name, float x, float y, float z);
    void setVec4(std::string_view name, const glm::vec4 &value);
    void setVec4(std::string_view name, float x, float y, float z, float w);

    void setMat2(std::string_view name, const glm::mat2 &value);
    void setMat3(std::string_view name, const glm::mat3 &value);
    void setMat4(std::string_view name, const glm::mat4 &value);

private:
    int getUniformLocation(std::string_view name);

    unsigned int m_id;
    std::unordered_map<std::string, int> m_uniformLocations;
};
