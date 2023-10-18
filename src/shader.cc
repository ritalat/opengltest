#include "shader.hh"

#include "glad/gl.h"
#include "glm/glm.hpp"

#include <fstream>
#include <string>
#include <string_view>

#define SHADER_PATH "../shaders/"

bool Shader::load(const std::string_view vert, const std::string_view frag)
{
    std::string vertPath(SHADER_PATH);
    vertPath += vert;
    std::ifstream vertFile(vertPath);
    std::string vertCode( (std::istreambuf_iterator<char>(vertFile)),
                          (std::istreambuf_iterator<char>()) );
    const char *vertCodeStr = vertCode.data();

    std::string fragPath(SHADER_PATH);
    fragPath += frag;
    std::ifstream fragFile(fragPath);
    std::string fragCode( (std::istreambuf_iterator<char>(fragFile)),
                          (std::istreambuf_iterator<char>()) );
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
        fprintf(stderr, "Failed to compile vertex shader: %s\n", infoLog);
        return false;
    }

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fragmentShader, 1, &fragCodeStr, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        fprintf(stderr, "Failed to compile fragment shader: %s\n", infoLog);
        return false;
    }

    id = glCreateProgram();

    glAttachShader(id, vertexShader);
    glAttachShader(id, fragmentShader);
    glLinkProgram(id);

    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(id, 512, NULL, infoLog);
        fprintf(stderr, "Failed to link shader program: %s\n", infoLog);
        return false;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return true;
}

void Shader::use()
{
    glUseProgram(id);
}

void Shader::setBool(const std::string_view name, bool value)
{
    int location = glGetUniformLocation(id, name.data());
    glUniform1i(location, (int)value);
}

void Shader::setInt(const std::string_view name, int value)
{
    int location = glGetUniformLocation(id, name.data());
    glUniform1i(location, value);
}

void Shader::setFloat(const std::string_view name, float value)
{
    int location = glGetUniformLocation(id, name.data());
    glUniform1i(location, value);
}

void Shader::setVec2(const std::string_view name, const glm::vec2 &value)
{
    int location = glGetUniformLocation(id, name.data());
    glUniform2fv(location, 1, &value[0]);
}

void Shader::setVec2(const std::string_view name, float x, float y)
{
    int location = glGetUniformLocation(id, name.data());
    glUniform2f(location, x, y);
}

void Shader::setVec3(const std::string_view name, const glm::vec3 &value)
{
    int location = glGetUniformLocation(id, name.data());
    glUniform3fv(location, 1, &value[0]);
}

void Shader::setVec3(const std::string_view name, float x, float y, float z)
{
    int location = glGetUniformLocation(id, name.data());
    glUniform3f(location, x, y, z);
}

void Shader::setVec4(const std::string_view name, const glm::vec4 &value)
{
    int location = glGetUniformLocation(id, name.data());
    glUniform4fv(location, 1, &value[0]);
}

void Shader::setVec4(const std::string_view name, float x, float y, float z, float w)
{
    int location = glGetUniformLocation(id, name.data());
    glUniform4f(location, x, y, z, w);
}

void Shader::setMat2(const std::string_view name, const glm::mat2 &value)
{
    int location = glGetUniformLocation(id, name.data());
    glUniformMatrix2fv(location, 1, GL_FALSE, &value[0][0]);
}

void Shader::setMat3(const std::string_view name, const glm::mat3 &value)
{
    int location = glGetUniformLocation(id, name.data());
    glUniformMatrix3fv(location, 1, GL_FALSE, &value[0][0]);
}

void Shader::setMat4(const std::string_view name, const glm::mat4 &value)
{
    int location = glGetUniformLocation(id, name.data());
    glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
}
