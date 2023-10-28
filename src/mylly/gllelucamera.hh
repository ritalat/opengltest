#pragma once

#include "gllelu.hh"

#include "glm/glm.hpp"
#include "SDL.h"

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
    float fov;
    float cameraSpeed;
    float mouseSensitivity;
    bool quit;
    int status;
};
