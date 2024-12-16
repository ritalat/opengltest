#include "shader.hh"

#include "path.hh"

#if defined(USE_GLES)
#include "glad/gles2.h"
#else
#include "glad/gl.h"
#endif
#include "glm/glm.hpp"

#include <cstdio>
#include <fstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>

Shader::Shader(std::string_view vert, std::string_view frag):
    m_id(0)
{
    std::string vertCode = slurpFile(getShaderPath(vert));
    if (vertCode.empty())
        throw std::runtime_error("Failed to read shader file: " + std::string(vert) + FILE_ERROR_HINT);
    const char *vertCodeStr = vertCode.data();

    std::string fragCode = slurpFile(getShaderPath(frag));
    if (fragCode.empty())
        throw std::runtime_error("Failed to read shader file: " + std::string(frag) + FILE_ERROR_HINT);
    const char *fragCodeStr = fragCode.data();

    int success;
    char infoLog[512];

    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertexShader, 1, &vertCodeStr, NULL);
    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        throw std::runtime_error("Failed to compile vertex shader: " + std::string(vert) + "\n" + std::string(infoLog));
    }

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fragmentShader, 1, &fragCodeStr, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        throw std::runtime_error("Failed to compile fragment shader: " + std::string(frag) + "\n" + std::string(infoLog));
    }

    m_id = glCreateProgram();

    glAttachShader(m_id, vertexShader);
    glAttachShader(m_id, fragmentShader);
    glLinkProgram(m_id);

    glGetProgramiv(m_id, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(m_id, 512, NULL, infoLog);
        throw std::runtime_error("Failed to link shader program: "
                                 + std::string(vert) + ", " + std::string(frag) + "\n"  + std::string(infoLog));
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

#if !(defined(USE_GLES))
Shader::Shader(std::string_view comp):
    m_id(0)
{
    std::string compCode = slurpFile(getShaderPath(comp));
    if (compCode.empty())
        throw std::runtime_error("Failed to read shader file: " + std::string(comp) + FILE_ERROR_HINT);
    const char *compCodeStr = compCode.data();

    int success;
    char infoLog[512];

    unsigned int computeShader;
    computeShader = glCreateShader(GL_COMPUTE_SHADER);

    glShaderSource(computeShader, 1, &compCodeStr, NULL);
    glCompileShader(computeShader);

    glGetShaderiv(computeShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(computeShader, 512, NULL, infoLog);
        throw std::runtime_error("Failed to compile compute shader: " + std::string(comp) + "\n" + std::string(infoLog));
    }

    m_id = glCreateProgram();

    glAttachShader(m_id, computeShader);
    glLinkProgram(m_id);

    glGetProgramiv(m_id, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(m_id, 512, NULL, infoLog);
        throw std::runtime_error("Failed to link shader program: " + std::string(comp) + "\n" + std::string(infoLog));
    }

    glDeleteShader(computeShader);
}
#endif

Shader::~Shader()
{
    if (m_id)
        glDeleteProgram(m_id);
}

void Shader::use()
{
    glUseProgram(m_id);
}

unsigned int Shader::id() const
{
    return m_id;
}

void Shader::setBool(std::string_view name, bool value)
{
    int location = getUniformLocation(name);
    glUniform1i(location, (int)value);
}

void Shader::setInt(std::string_view name, int value)
{
    int location = getUniformLocation(name);
    glUniform1i(location, value);
}

void Shader::setFloat(std::string_view name, float value)
{
    int location = getUniformLocation(name);
    glUniform1f(location, value);
}

void Shader::setVec2(std::string_view name, const glm::vec2 &value)
{
    int location = getUniformLocation(name);
    glUniform2fv(location, 1, &value[0]);
}

void Shader::setVec2(std::string_view name, float x, float y)
{
    int location = getUniformLocation(name);
    glUniform2f(location, x, y);
}

void Shader::setVec3(std::string_view name, const glm::vec3 &value)
{
    int location = getUniformLocation(name);
    glUniform3fv(location, 1, &value[0]);
}

void Shader::setVec3(std::string_view name, float x, float y, float z)
{
    int location = getUniformLocation(name);
    glUniform3f(location, x, y, z);
}

void Shader::setVec4(std::string_view name, const glm::vec4 &value)
{
    int location = getUniformLocation(name);
    glUniform4fv(location, 1, &value[0]);
}

void Shader::setVec4(std::string_view name, float x, float y, float z, float w)
{
    int location = getUniformLocation(name);
    glUniform4f(location, x, y, z, w);
}

void Shader::setMat2(std::string_view name, const glm::mat2 &value)
{
    int location = getUniformLocation(name);
    glUniformMatrix2fv(location, 1, GL_FALSE, &value[0][0]);
}

void Shader::setMat3(std::string_view name, const glm::mat3 &value)
{
    int location = getUniformLocation(name);
    glUniformMatrix3fv(location, 1, GL_FALSE, &value[0][0]);
}

void Shader::setMat4(std::string_view name, const glm::mat4 &value)
{
    int location = getUniformLocation(name);
    glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
}

int Shader::getUniformLocation(std::string_view name)
{
    int location = 0;
    std::string nameStr(name);
    auto iter = m_uniformLocations.find(nameStr);
    if (iter == m_uniformLocations.end()) {
        location = glGetUniformLocation(m_id, name.data());
        m_uniformLocations[nameStr] = location;
    } else {
        location = iter->second;
    }
    return location;
}
