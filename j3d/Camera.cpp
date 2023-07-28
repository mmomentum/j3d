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
    glm::vec3 rightDirection = { sin(yaw + (3.1415 / 2.0)) * cos(pitch),sin(pitch),cos(yaw + (3.1415 / 2.0)) * cos(pitch) };
    position += amount * rightDirection;
}

void camera::turn(float deltaYaw, float deltaPitch)
{
    yaw += deltaYaw;
    pitch += deltaPitch;

    if (fabs(yaw) > 6.28)
        yaw += (yaw > 0) ? -6.28 : 6.28;
    if (fabs(pitch) > 6.28)
        pitch += (pitch > 0) ? -6.28 : 6.28;
    direction = { sin(yaw) * cos(pitch),sin(pitch),cos(yaw) * cos(pitch) };
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
