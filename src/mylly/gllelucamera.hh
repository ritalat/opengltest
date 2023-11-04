#pragma once

#include "gllelu.hh"

#include "glm/glm.hpp"
#include "SDL.h"

struct Camera
{
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);;
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    float fov = 45.0f;
    float sensitivity = 0.1f;
    float speed = 0.0025f;
    float yaw = -90.0f;
    float pitch = 0.0f;
};

class GLleluCamera: public GLlelu
{
public:
    GLleluCamera(int argc, char *argv[]);
    virtual ~GLleluCamera();
    virtual int main_loop() final;
    virtual void event(SDL_Event &event);
    virtual void keystate(const Uint8 *keystate);
    virtual void update(unsigned int deltaTime);
    virtual void render() = 0;

    glm::mat4 view;
    Camera camera;
    bool quit;
    int status;
};
