/*
  ==============================================================================

    Camera.h
    Created: 8 Jul 2023 1:33:46am
    Author:  Syerjchep

  ==============================================================================
*/

#pragma once

#include "Common.h"
#include "Program.h"

struct camera
{
    public:
        float pitch = 0.0;
        float yaw = 0.0;

        glm::vec3 position = glm::vec3(0, 0, 0);
        glm::vec3 direction = glm::vec3(0, 0, 1);
        glm::vec3 up = glm::vec3(0, 1, 0);

        void turn(float deltaYaw, float deltaPitch);
        void moveForward(float amount);
        void moveRight(float amount);
        virtual const void render(program *currentProgram) = 0;
};

struct perspectiveCamera : camera
{
    //Degrees:
    float fov = 90.0;
    float aspectRatio = 1.0;
    float nearPlane = 0.05;
    float farPlane = 100.0;

    const void render(program* currentProgram);
};

struct orthoCamera : camera
{
    glm::vec3 near = glm::vec3(-10, -10, -10);
    glm::vec3 far = glm::vec3(10, 10, 10);

    const void render(program* currentProgram);
};