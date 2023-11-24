#include "gllelucamera.hh"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "SDL.h"

#include <cmath>
#include <cstdlib>

GLleluCamera::GLleluCamera(int argc, char *argv[], GLVersion glVersion):
    GLlelu(argc, argv, glVersion)
{
}

GLleluCamera::~GLleluCamera()
{
}

int GLleluCamera::main_loop()
{
    unsigned int deltaTime = 0;
    unsigned int lastFrame = 0;

    bool quit = false;
    SDL_Event eventStruct;

    while (!quit) {
        unsigned int currentFrame = SDL_GetTicks();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        float cameraSpeedScaled = m_camera.speed * deltaTime;

        while (SDL_PollEvent(&eventStruct)) {
            switch (eventStruct.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_KEYUP:
                    if (SDL_SCANCODE_ESCAPE == eventStruct.key.keysym.scancode)
                        quit = true;
                    break;
                case SDL_MOUSEMOTION:
                    if (SDL_BUTTON_LMASK & eventStruct.motion.state) {
                        m_camera.yaw += eventStruct.motion.xrel * m_camera.sensitivity;
                        m_camera.pitch -= eventStruct.motion.yrel * m_camera.sensitivity;
                        if (m_camera.pitch > 89.0f)
                            m_camera.pitch = 89.0f;
                        if (m_camera.pitch < -89.0f)
                            m_camera.pitch = -89.0f;
                    }
                    break;
                case SDL_MOUSEWHEEL:
                    m_camera.fov -= float(eventStruct.wheel.y);
                    if (m_camera.fov < 1.0f)
                        m_camera.fov = 1.0f;
                    if (m_camera.fov > 100.0f)
                        m_camera.fov = 100.0f;
                    break;
                default:
                    break;
            }
            Status ret = event(eventStruct);
            if (ret != Status::Ok)
                return ret == Status::QuitSuccess ? EXIT_SUCCESS : EXIT_FAILURE;
        }

        m_camera.front = glm::normalize(glm::vec3(
            cos(glm::radians(m_camera.yaw) * cos(glm::radians(m_camera.pitch))),
            sin(glm::radians(m_camera.pitch)),
            sin(glm::radians(m_camera.yaw)) * cos(glm::radians(m_camera.pitch))
        ));

        const Uint8 *keystateArray = SDL_GetKeyboardState(NULL);
        if (keystateArray[SDL_SCANCODE_W]) {
            m_camera.position += cameraSpeedScaled * m_camera.front;
        }
        if (keystateArray[SDL_SCANCODE_A]) {
            m_camera.position -= glm::normalize(glm::cross(m_camera.front, m_camera.up)) * cameraSpeedScaled;
        }
        if (keystateArray[SDL_SCANCODE_S]) {
            m_camera.position -= cameraSpeedScaled * m_camera.front;
        }
        if (keystateArray[SDL_SCANCODE_D]) {
            m_camera.position += glm::normalize(glm::cross(m_camera.front, m_camera.up)) * cameraSpeedScaled;
        }
        Status ret = keystate(keystateArray);
        if (ret != Status::Ok)
            return ret == Status::QuitSuccess ? EXIT_SUCCESS : EXIT_FAILURE;

        m_view = glm::lookAt(m_camera.position, m_camera.position + m_camera.front, m_camera.up);
        m_projection = glm::perspective(glm::radians(m_camera.fov), (float)m_fbSize.width / (float)m_fbSize.height, 0.1f, 100.0f);

        ret = update(deltaTime);
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

Status GLleluCamera::keystate(const Uint8 *keystate)
{
    (void)keystate;
    return Status::Ok;
}

Status GLleluCamera::update(unsigned int deltaTime)
{
    (void)deltaTime;
    return Status::Ok;
}
