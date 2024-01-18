#pragma once

#include "gllelu.hh"

#include "glm/glm.hpp"
#include "SDL.h"

enum class Status
{
    QuitFailure = -1,
    Ok = 0,
    QuitSuccess = 1
};

struct Camera
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
};

class GLleluCamera: public GLlelu
{
public:
    GLleluCamera(int argc, char *argv[], GLVersion glVersion = GLVersion::GL33);
    virtual ~GLleluCamera();
    virtual int main_loop() final;
    virtual Status event(SDL_Event &event);
    virtual Status keystate(const Uint8 *keystate);
    virtual Status update(unsigned int deltaTime);
    virtual Status render() = 0;

    glm::mat4 m_view;
    glm::mat4 m_projection;
    Camera m_camera;
    SDL_GameController *gamepad;
    SDL_JoystickID gamepadId;
};
