#include "gllelucamera.hh"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "SDL.h"

#include <cmath>

GLleluCamera::GLleluCamera(int argc, char *argv[]):
    GLlelu(argc, argv),
    cameraPos(0.0f, 0.0f, 3.0f),
    fov(45.0f),
    mouseSensitivity(0.1f),
    cameraSpeed(0.0025f),
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

    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    float yaw = -90.0f;
    float pitch = 0.0f;

    SDL_Event eventStruct;

    while (!quit) {
        unsigned int currentFrame = SDL_GetTicks();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        float cameraSpeedScaled = cameraSpeed * deltaTime;

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
                        yaw += eventStruct.motion.xrel * mouseSensitivity;
                        pitch -= eventStruct.motion.yrel * mouseSensitivity;
                        if (pitch > 89.0f)
                            pitch = 89.0f;
                        if (pitch < -89.0f)
                            pitch = -89.0f;
                    }
                    break;
                case SDL_MOUSEWHEEL:
                    fov -= float(eventStruct.wheel.y);
                    if (fov < 1.0f)
                        fov = 1.0f;
                    if (fov > 100.0f)
                        fov = 100.0f;
                    break;
                default:
                    break;
            }
            event(eventStruct);
        }

        cameraFront = glm::normalize(glm::vec3(
            cos(glm::radians(yaw) * cos(glm::radians(pitch))),
            sin(glm::radians(pitch)),
            sin(glm::radians(yaw)) * cos(glm::radians(pitch))
        ));

        const Uint8 *keystateArray = SDL_GetKeyboardState(NULL);
        if (keystateArray[SDL_SCANCODE_W]) {
            cameraPos += cameraSpeedScaled * cameraFront;
        }
        if (keystateArray[SDL_SCANCODE_A]) {
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeedScaled;
        }
        if (keystateArray[SDL_SCANCODE_S]) {
            cameraPos -= cameraSpeedScaled * cameraFront;
        }
        if (keystateArray[SDL_SCANCODE_D]) {
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeedScaled;
        }
        keystate(keystateArray);

        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        
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
