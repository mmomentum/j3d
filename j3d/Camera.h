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

struct perspectiveCamera : sceneObject
{
    //Degrees:
    float fov = 90.0;
    float aspectRatio = 1.0;
    float nearPlane = 0.05;
    float farPlane = 100.0;

    const void render(program* currentProgram);
};

struct orthoCamera : sceneObject
{
    glm::vec3 near = glm::vec3(-10, -10, -10);
    glm::vec3 far = glm::vec3(10, 10, 10);

    const void render(program* currentProgram);
};