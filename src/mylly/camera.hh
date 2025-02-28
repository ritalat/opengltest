#pragma once

#include "api_decl.hh"
#include "gllelu.hh"

#include "glm/glm.hpp"
#include "SDL3/SDL.h"

#include <cstdint>

class MYLLY_API Camera
{
public:
    Camera(GLlelu *parent);
    void event(SDL_Event *event);
    void iterate();
    const glm::mat4 &view() const;
    const glm::mat4 &projection() const;

    struct CameraData
    {
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f);
        glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
        float fov = 45.0f;
        float sensitivity = 0.1f;
        float speed = 0.0025f;
        float yaw = -90.0f;
        float pitch = 0.0f;
        int deadzone = 2000;
    };
    CameraData data;

private:
    GLlelu *m_parent;
    glm::mat4 m_view;
    glm::mat4 m_projection;
    uint64_t m_deltaTime;
    uint64_t m_lastFrame;
};
