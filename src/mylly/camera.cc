#include "camera.hh"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "SDL3/SDL.h"

#include <cmath>
#include <cstdint>
#include <cstdlib>

Camera::Camera(GLlelu *parent):
    m_parent(parent),
    m_deltaTime(0),
    m_lastFrame(0)
{
}

void Camera::event(SDL_Event *event)
{
    switch (event->type) {
        case SDL_EVENT_MOUSE_MOTION:
            if (m_parent->windowGrab() || SDL_BUTTON_LMASK & event->motion.state) {
                data.yaw += static_cast<float>(event->motion.xrel) * data.sensitivity;
                data.pitch -= static_cast<float>(event->motion.yrel) * data.sensitivity;
                if (data.pitch > 89.0f)
                    data.pitch = 89.0f;
                if (data.pitch < -89.0f)
                    data.pitch = -89.0f;
            }
            break;

        case SDL_EVENT_MOUSE_WHEEL:
            data.fov -= float(event->wheel.y);
            if (data.fov < 1.0f)
                data.fov = 1.0f;
            if (data.fov > 100.0f)
                data.fov = 100.0f;
            break;

        default:
            break;
    }
}

void Camera::iterate()
{
    uint64_t currentFrame = SDL_GetTicks();
    m_deltaTime = currentFrame - m_lastFrame;
    m_lastFrame = currentFrame;
    float deltaf = static_cast<float>(m_deltaTime);
    float cameraSpeedScaled = data.speed * deltaf;
    float cameraSensitivityScaled = data.sensitivity * deltaf;

    data.front = glm::normalize(glm::vec3(
        cos(glm::radians(data.yaw)) * cos(glm::radians(data.pitch)),
        sin(glm::radians(data.pitch)),
        sin(glm::radians(data.yaw)) * cos(glm::radians(data.pitch))
    ));
    data.right = glm::normalize(glm::cross(data.front, data.worldUp));
    data.up = glm::normalize(glm::cross(data.right, data.front));

    bool keyboardMovement = false;
    const bool *keystates = SDL_GetKeyboardState(NULL);

    if (keystates[SDL_SCANCODE_LSHIFT]) {
        cameraSpeedScaled *= 2.0f;
        keyboardMovement = true;
    }
    if (keystates[SDL_SCANCODE_W]) {
        data.position += cameraSpeedScaled * data.front;
        keyboardMovement = true;
    }
    if (keystates[SDL_SCANCODE_A]) {
        data.position -= cameraSpeedScaled * data.right;
        keyboardMovement = true;
    }
    if (keystates[SDL_SCANCODE_S]) {
        data.position -= cameraSpeedScaled * data.front;
        keyboardMovement = true;
    }
    if (keystates[SDL_SCANCODE_D]) {
        data.position += cameraSpeedScaled * data.right;
        keyboardMovement = true;
    }
    if (keystates[SDL_SCANCODE_SPACE]) {
        data.position += cameraSpeedScaled * data.worldUp;
        keyboardMovement = true;
    }
    if (keystates[SDL_SCANCODE_LCTRL]) {
        data.position -= cameraSpeedScaled * data.worldUp;
        keyboardMovement = true;
    }

    SDL_Gamepad *gamepad = m_parent->gamepad();

    if (gamepad && !keyboardMovement) {
        if (SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_SOUTH)) {
            cameraSpeedScaled *= 2.0f;
        }
        if (SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER)) {
            data.fov -= (0.1f * deltaf);
        }
        if (SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_LEFT_SHOULDER)) {
            data.fov += (0.1f * deltaf);
        }
        if (data.fov < 1.0f) {
            data.fov = 1.0f;
        }
        if (data.fov > 100.0f) {
            data.fov = 100.0f;
        }

        auto scale_axis = [](Sint16 axis){ return axis / 32767.0f; };
        Sint16 axis = 0;

        axis = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER);
        if (axis > data.deadzone) {
            data.position += cameraSpeedScaled * data.worldUp * scale_axis(axis);
        }
        axis = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFT_TRIGGER);
        if (axis > data.deadzone) {
            data.position -= cameraSpeedScaled * data.worldUp * scale_axis(axis);
        }

        axis = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTX);
        if (abs(axis) > data.deadzone) {
            data.position += cameraSpeedScaled * data.right * scale_axis(axis);
        }
        axis = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTY);
        if (abs(axis) > data.deadzone) {
            data.position -= cameraSpeedScaled * data.front * scale_axis(axis);
        }

        axis = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_RIGHTX);
        if (abs(axis) > data.deadzone) {
            data.yaw += cameraSensitivityScaled * scale_axis(axis);
        }
        axis = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_RIGHTY);
        if (abs(axis) > data.deadzone) {
            data.pitch -= cameraSensitivityScaled * scale_axis(axis);
        }
        if (data.pitch > 89.0f) {
            data.pitch = 89.0f;
        }
        if (data.pitch < -89.0f) {
            data.pitch = -89.0f;
        }
    }

    m_view = glm::lookAt(data.position, data.position + data.front, data.up);
    m_projection = glm::perspective(glm::radians(data.fov),
                                    static_cast<float>(m_parent->fbSize().width) / static_cast<float>(m_parent->fbSize().height),
                                    0.1f, 100.0f);
}

const glm::mat4 &Camera::view() const
{
    return m_view;
}

const glm::mat4 &Camera::projection() const
{
    return m_projection;
}
