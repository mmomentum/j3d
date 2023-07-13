/*
  ==============================================================================

    Camera.cpp
    Created: 8 Jul 2023 1:33:43am
    Author:  Syerjchep

  ==============================================================================
*/

#include "Camera.h"

const void perspectiveCamera::render(program* currentProgram)
{
    currentProgram->matrix_cameraProjection = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
    glm::vec3 direction = glm::toMat4(rotation) * glm::vec4(0, 0, 1, 0);
    currentProgram->matrix_cameraView = glm::lookAt(position, position + direction, glm::vec3(0, 1, 0));
}

const void orthoCamera::render(program* currentProgram)
{
    currentProgram->matrix_cameraProjection = glm::ortho(near.x, near.y, near.z, far.x, far.y, far.z);
    glm::vec3 direction = glm::toMat4(rotation) * glm::vec4(0, 0, 1, 0);
    currentProgram->matrix_cameraView = glm::lookAt(position, position + direction, glm::vec3(0, 1, 0));
}