#include "gllelucamera.hh"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "SDL.h"

#include <cmath>

GLleluCamera::GLleluCamera(int argc, char *argv[]):
    GLlelu(argc, argv),
    quit(false),
    status(EXIT_SUCCESS)
{
}

GLleluCamera::~GLleluCamera()
{
}

int GLleluCamera::main_loop()
{
    unsigned int deltaTime = 0;
    unsigned int lastFrame = 0;

    SDL_Event eventStruct;

    while (!quit) {
        unsigned int currentFrame = SDL_GetTicks();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        float cameraSpeedScaled = camera.speed * deltaTime;

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
                        camera.yaw += eventStruct.motion.xrel * camera.sensitivity;
                        camera.pitch -= eventStruct.motion.yrel * camera.sensitivity;
                        if (camera.pitch > 89.0f)
                            camera.pitch = 89.0f;
                        if (camera.pitch < -89.0f)
                            camera.pitch = -89.0f;
                    }
                    break;
                case SDL_MOUSEWHEEL:
                    camera.fov -= float(eventStruct.wheel.y);
                    if (camera.fov < 1.0f)
                        camera.fov = 1.0f;
                    if (camera.fov > 100.0f)
                        camera.fov = 100.0f;
                    break;
                default:
                    break;
            }
            event(eventStruct);
        }

        camera.front = glm::normalize(glm::vec3(
            cos(glm::radians(camera.yaw) * cos(glm::radians(camera.pitch))),
            sin(glm::radians(camera.pitch)),
            sin(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch))
        ));

        const Uint8 *keystateArray = SDL_GetKeyboardState(NULL);
        if (keystateArray[SDL_SCANCODE_W]) {
            camera.position += cameraSpeedScaled * camera.front;
        }
        if (keystateArray[SDL_SCANCODE_A]) {
            camera.position -= glm::normalize(glm::cross(camera.front, camera.up)) * cameraSpeedScaled;
        }
        if (keystateArray[SDL_SCANCODE_S]) {
            camera.position -= cameraSpeedScaled * camera.front;
        }
        if (keystateArray[SDL_SCANCODE_D]) {
            camera.position += glm::normalize(glm::cross(camera.front, camera.up)) * cameraSpeedScaled;
        }
        keystate(keystateArray);

        view = glm::lookAt(camera.position, camera.position + camera.front, camera.up);
        
        update(deltaTime);

        render();
    }

    return status;
}

void GLleluCamera::event(SDL_Event &event)
{
    (void)event;
}

void GLleluCamera::keystate(const Uint8 *keystate)
{
    (void)keystate;
}

void GLleluCamera::update(unsigned int deltaTime)
{
    (void)deltaTime;
}
