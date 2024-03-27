#include "gllelucamera.hh"

#if defined(__EMSCRIPTEN__) || defined(USE_GLES)
#include "glad/gles2.h"
#else
#include "glad/gl.h"
#endif
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "SDL.h"

#include <cmath>
#include <cstdlib>

GLleluCamera::GLleluCamera(int argc, char *argv[], GLVersion glVersion):
    GLlelu(argc, argv, glVersion),
    m_gamepad(NULL),
    m_gamepadId(-1)
{
    find_gamepad();
}

GLleluCamera::~GLleluCamera()
{
    if (m_gamepad)
        SDL_GameControllerClose(m_gamepad);
}

int GLleluCamera::main_loop()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    unsigned int deltaTime = 0;
    unsigned int lastFrame = 0;

    bool quit = false;
    SDL_Event e;

    while (!quit) {
        unsigned int currentFrame = SDL_GetTicks();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        float cameraSpeedScaled = m_camera.speed * deltaTime;
        float cameraSensitivityScaled = m_camera.sensitivity * deltaTime;

        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_QUIT:
                    quit = true;
                    break;

                case SDL_KEYUP:
                    if (SDL_SCANCODE_ESCAPE == e.key.keysym.scancode)
                        quit = true;
                    if (SDL_SCANCODE_F == e.key.keysym.scancode)
                        window_fullscreen(!m_fullscreen);
                    if (SDL_SCANCODE_G == e.key.keysym.scancode)
                        window_grab(!m_mouseGrab);
                    break;

                case SDL_MOUSEMOTION:
                    if (m_mouseGrab || SDL_BUTTON_LMASK & e.motion.state) {
                        m_camera.yaw += e.motion.xrel * m_camera.sensitivity;
                        m_camera.pitch -= e.motion.yrel * m_camera.sensitivity;
                        if (m_camera.pitch > 89.0f)
                            m_camera.pitch = 89.0f;
                        if (m_camera.pitch < -89.0f)
                            m_camera.pitch = -89.0f;
                    }
                    break;

                case SDL_MOUSEWHEEL:
                    m_camera.fov -= float(e.wheel.y);
                    if (m_camera.fov < 1.0f)
                        m_camera.fov = 1.0f;
                    if (m_camera.fov > 100.0f)
                        m_camera.fov = 100.0f;
                    break;

                case SDL_CONTROLLERBUTTONUP:
                    if (m_gamepad && e.cbutton.which == m_gamepadId) {
                        if (SDL_CONTROLLER_BUTTON_B == e.cbutton.button)
                            quit = true;
                        if (SDL_CONTROLLER_BUTTON_Y == e.cbutton.button)
                            window_fullscreen(!m_fullscreen);
                    }
                    break;

                case SDL_CONTROLLERDEVICEADDED:
                    if (!m_gamepad) {
                        m_gamepad = SDL_GameControllerOpen(e.cdevice.which);
                        m_gamepadId = SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(m_gamepad));
                    }
                    break;

                case SDL_CONTROLLERDEVICEREMOVED:
                    if (m_gamepad && e.cdevice.which == m_gamepadId) {
                        SDL_GameControllerClose(m_gamepad);
                        m_gamepad = NULL;
                        m_gamepadId = -1;
                        find_gamepad();
                    }
                    break;

                default:
                    break;
            }

            Status ret = event(e);
            if (ret != Status::Ok)
                return ret == Status::QuitSuccess ? EXIT_SUCCESS : EXIT_FAILURE;
        }

        m_camera.front = glm::normalize(glm::vec3(
            cos(glm::radians(m_camera.yaw)) * cos(glm::radians(m_camera.pitch)),
            sin(glm::radians(m_camera.pitch)),
            sin(glm::radians(m_camera.yaw)) * cos(glm::radians(m_camera.pitch))
        ));
        m_camera.right = glm::normalize(glm::cross(m_camera.front, m_camera.worldUp));
        m_camera.up = glm::normalize(glm::cross(m_camera.right, m_camera.front));

        bool keyboardMovement = false;
        const Uint8 *keystates = SDL_GetKeyboardState(NULL);

        if (keystates[SDL_SCANCODE_LSHIFT]) {
            cameraSpeedScaled *= 2.0f;
            keyboardMovement = true;
        }
        if (keystates[SDL_SCANCODE_W]) {
            m_camera.position += cameraSpeedScaled * m_camera.front;
            keyboardMovement = true;
        }
        if (keystates[SDL_SCANCODE_A]) {
            m_camera.position -= cameraSpeedScaled * m_camera.right;
            keyboardMovement = true;
        }
        if (keystates[SDL_SCANCODE_S]) {
            m_camera.position -= cameraSpeedScaled * m_camera.front;
            keyboardMovement = true;
        }
        if (keystates[SDL_SCANCODE_D]) {
            m_camera.position += cameraSpeedScaled * m_camera.right;
            keyboardMovement = true;
        }
        if (keystates[SDL_SCANCODE_SPACE]) {
            m_camera.position += cameraSpeedScaled * m_camera.worldUp;
            keyboardMovement = true;
        }
        if (keystates[SDL_SCANCODE_LCTRL]) {
            m_camera.position -= cameraSpeedScaled * m_camera.worldUp;
            keyboardMovement = true;
        }

        if (m_gamepad && !keyboardMovement) {
            if (SDL_GameControllerGetButton(m_gamepad, SDL_CONTROLLER_BUTTON_A)) {
                cameraSpeedScaled *= 2.0f;
            }
            if (SDL_GameControllerGetButton(m_gamepad, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER)) {
                m_camera.fov -= (0.1f * deltaTime);
            }
            if (SDL_GameControllerGetButton(m_gamepad, SDL_CONTROLLER_BUTTON_LEFTSHOULDER)) {
                m_camera.fov += (0.1f * deltaTime);
            }
            if (m_camera.fov < 1.0f) {
                m_camera.fov = 1.0f;
            }
            if (m_camera.fov > 100.0f) {
                m_camera.fov = 100.0f;
            }

            auto scale_axis = [](Sint16 axis){ return axis / 32767.0f; };
            Sint16 axis = 0;

            axis = SDL_GameControllerGetAxis(m_gamepad, SDL_CONTROLLER_AXIS_TRIGGERRIGHT);
            if (axis > m_camera.deadzone) {
                m_camera.position += cameraSpeedScaled * m_camera.worldUp * scale_axis(axis);
            }
            axis = SDL_GameControllerGetAxis(m_gamepad, SDL_CONTROLLER_AXIS_TRIGGERLEFT);
            if (axis > m_camera.deadzone) {
                m_camera.position -= cameraSpeedScaled * m_camera.worldUp * scale_axis(axis);
            }

            axis = SDL_GameControllerGetAxis(m_gamepad, SDL_CONTROLLER_AXIS_LEFTX);
            if (abs(axis) > m_camera.deadzone) {
                m_camera.position += cameraSpeedScaled * m_camera.right * scale_axis(axis);
            }
            axis = SDL_GameControllerGetAxis(m_gamepad, SDL_CONTROLLER_AXIS_LEFTY);
            if (abs(axis) > m_camera.deadzone) {
                m_camera.position -= cameraSpeedScaled * m_camera.front * scale_axis(axis);
            }

            axis = SDL_GameControllerGetAxis(m_gamepad, SDL_CONTROLLER_AXIS_RIGHTX);
            if (abs(axis) > m_camera.deadzone) {
                m_camera.yaw += cameraSensitivityScaled * scale_axis(axis);
            }
            axis = SDL_GameControllerGetAxis(m_gamepad, SDL_CONTROLLER_AXIS_RIGHTY);
            if (abs(axis) > m_camera.deadzone) {
                m_camera.pitch -= cameraSensitivityScaled * scale_axis(axis);
            }
            if (m_camera.pitch > 89.0f) {
                m_camera.pitch = 89.0f;
            }
            if (m_camera.pitch < -89.0f) {
                m_camera.pitch = -89.0f;
            }
        }

        m_view = glm::lookAt(m_camera.position, m_camera.position + m_camera.front, m_camera.up);
        m_projection = glm::perspective(glm::radians(m_camera.fov), (float)m_fbSize.width / (float)m_fbSize.height, 0.1f, 100.0f);

        Status ret = update(deltaTime);
        if (ret != Status::Ok)
            return ret == Status::QuitSuccess ? EXIT_SUCCESS : EXIT_FAILURE;

        ret = render();
        if (ret != Status::Ok)
            return ret == Status::QuitSuccess ? EXIT_SUCCESS : EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

Status GLleluCamera::event(SDL_Event &event)
{
    (void)event;
    return Status::Ok;
}

Status GLleluCamera::update(unsigned int deltaTime)
{
    (void)deltaTime;
    return Status::Ok;
}

void GLleluCamera::find_gamepad()
{
    int numJoysticks = SDL_NumJoysticks();
    for (int i = 0; i < numJoysticks; ++i) {
        if (SDL_IsGameController(i)) {
            m_gamepad = SDL_GameControllerOpen(i);
            m_gamepadId = SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(m_gamepad));
            break;
        }
    }
}
