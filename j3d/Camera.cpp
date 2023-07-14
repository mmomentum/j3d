/*
  ==============================================================================

    Camera.cpp
    Created: 8 Jul 2023 1:33:43am
    Author:  Syerjchep

  ==============================================================================
*/

#include "Camera.h"

void camera::moveForward(float amount)
{
    position += amount * direction;
}

void camera::moveRight(float amount)
{
    glm::vec3 rightDirection = { sin(lastYaw + (3.1415 / 2.0)) * cos(lastPitch),sin(lastPitch),cos(lastYaw + (3.1415 / 2.0)) * cos(lastPitch) };
    position += amount * rightDirection;
}

void camera::endTurn()
{
    lastYaw += yawSinceTurn;
    lastPitch = pitchSinceTurn;
    yawSinceTurn = 0;
    pitchSinceTurn = 0;

    /*lastYaw = fmod(lastYaw, 3.1415 * 2.0);
    lastPitch = fmod(lastPitch, 3.1415 * 2.0);*/
    if (fabs(lastYaw) > 6.28)
        lastYaw += (lastYaw > 0) ? -6.28 : 6.28;
    if (fabs(lastPitch) > 6.28)
        lastPitch += (lastPitch > 0) ? -6.28 : 6.28;
}

void camera::turn(float deltaYaw, float deltaPitch)
{
    yawSinceTurn = deltaYaw;
    pitchSinceTurn = deltaPitch;
    direction = { sin(lastYaw+yawSinceTurn) * cos(lastPitch+pitchSinceTurn),sin(lastPitch + pitchSinceTurn),cos(lastYaw + yawSinceTurn) * cos(lastPitch + pitchSinceTurn) };
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
