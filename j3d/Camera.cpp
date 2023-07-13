/*
  ==============================================================================

    Camera.cpp
    Created: 8 Jul 2023 1:33:43am
    Author:  Syerjchep

  ==============================================================================
*/

#include "Camera.h"

void camera::turn(float deltaYaw, float deltaPitch)
{
    lastYaw += deltaYaw;
    lastPitch += deltaPitch;
    lastYaw = fmod(lastYaw, 3.1415 * 2.0);
    lastPitch = fmod(lastPitch, 3.1415 * 2.0);
    direction = { sin(deltaYaw) * cos(deltaPitch),sin(deltaPitch),cos(deltaYaw) * cos(deltaPitch) };
}

const void perspectiveCamera::render(program* currentProgram)
{
    currentProgram->matrix_cameraProjection = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
    currentProgram->matrix_cameraView = glm::lookAt(position, position + direction, glm::vec3(0, 1, 0));
}

const void orthoCamera::render(program* currentProgram)
{
    currentProgram->matrix_cameraProjection = glm::ortho(near.x, far.x, near.y, far.y, near.z, far.z);
    currentProgram->matrix_cameraView = glm::lookAt(position, position + direction, glm::vec3(0, 1, 0));
}